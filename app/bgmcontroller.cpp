#include "bgmcontroller.h"

#include <QAudioOutput>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QStandardPaths>
#include <QUrl>

BGMController* BGMController::instance()
{
    // 函数内静态对象：首次调用时构造，线程安全（C++11 起），程序退出时自动析构
    static BGMController s_instance;
    return &s_instance;
}

BGMController::BGMController(QObject* parent)
    : QObject(parent)
{
    loadPlayList();

    // 音频释放目录（Qt6 FFmpeg 后端不支持直接播 qrc，见 toPlayableUrl 注释）
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_audioTempDir = QDir(base).filePath(QStringLiteral("audio"));
    QDir().mkpath(m_audioTempDir);

    // 背景音乐一路、音效一路：两路独立播放，出牌语音不会打断 BGM
    m_bgmOutput = new QAudioOutput(this);
    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmPlayer->setAudioOutput(m_bgmOutput);

    m_effectOutput = new QAudioOutput(this);
    m_effectPlayer = new QMediaPlayer(this);
    m_effectPlayer->setAudioOutput(m_effectOutput);

    // 用 EndOfMedia 回调实现循环，兼容性比 setLoops 好
    connect(m_bgmPlayer, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && m_hasBgm) {
            m_bgmPlayer->stop();
            m_bgmPlayer->play();
        }
    });

    applyAudioState();
}

BGMController::~BGMController()
{
    m_bgmPlayer->stop();
    m_effectPlayer->stop();
}

void BGMController::loadPlayList()
{
    QFile file(QStringLiteral(":/conf/playList.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return;
    }

    QJsonObject root = doc.object();
    for (auto it = root.begin(); it != root.end(); ++it) {
        QStringList paths;
        const QJsonArray arr = it.value().toArray();
        for (const QJsonValue& v : arr) {
            const QString path = toResourcePath(v.toString());
            paths.append(path);
            // 建立 "Man_zhadan" -> ":/music/Man_zhadan.mp3" 的索引，供 playEffect 按名查找
            m_pathIndex.insert(QFileInfo(path).completeBaseName(), path);
        }
        m_groups.insert(it.key(), paths);
    }
}

QString BGMController::toResourcePath(const QString& path)
{
    // json 里写的是 "qrc:/music/x.mp3"，内部统一用 ":/music/x.mp3" 表示资源
    if (path.startsWith(QStringLiteral("qrc:/"))) {
        return path.mid(4).prepend(QLatin1Char(':'));
    }
    return path;
}

QUrl BGMController::toPlayableUrl(const QString& resPath)
{
    if (resPath.isEmpty()) {
        return QUrl();
    }

    // 命中缓存直接返回
    auto cached = m_extracted.constFind(resPath);
    if (cached != m_extracted.constEnd()) {
        return QUrl::fromLocalFile(cached.value());
    }

    QFile res(resPath);
    if (!res.exists()) {
        return QUrl();
    }

    // 非资源路径（已是真实文件）直接用
    if (!resPath.startsWith(QLatin1Char(':'))) {
        return QUrl::fromLocalFile(QFileInfo(resPath).absoluteFilePath());
    }

    // 释放到临时目录：文件名带上资源路径的哈希前缀，避免同名覆盖
    const QString fileName = QStringLiteral("%1_%2")
                                 .arg(qHash(resPath))
                                 .arg(QFileInfo(resPath).fileName());
    const QString target = QDir(m_audioTempDir).filePath(fileName);

    if (!QFile::exists(target) && !res.copy(target)) {
        return QUrl();
    }

    m_extracted.insert(resPath, target);
    return QUrl::fromLocalFile(target);
}

QString BGMController::resolveScene(Scene scene) const
{
    switch (scene) {
    case Scene::Welcome:
        return QStringLiteral(":/music/MusicEx_Welcome.mp3");
    case Scene::Normal:
        return QStringLiteral(":/music/MusicEx_Normal.mp3");
    case Scene::Exciting:
        return QStringLiteral(":/music/MusicEx_Exciting.mp3");
    case Scene::Win:
        return QStringLiteral(":/music/MusicEx_Win.mp3");
    case Scene::Lose:
        return QStringLiteral(":/music/MusicEx_Lose.mp3");
    }
    return QString();
}

QString BGMController::resolveEffect(const QString& name) const
{
    // 优先查 json 索引；查不到则按命名约定直接拼路径兜底
    auto it = m_pathIndex.constFind(name);
    if (it != m_pathIndex.constEnd()) {
        return it.value();
    }
    return QStringLiteral(":/music/%1.mp3").arg(name);
}

void BGMController::playBgm(Scene scene)
{
    // 同一场景重复调用不重启，避免音乐从头开始
    if (m_hasBgm && m_currentScene == scene) {
        return;
    }

    const QUrl url = toPlayableUrl(resolveScene(scene));
    if (url.isEmpty()) {
        return;
    }

    m_currentScene = scene;
    m_hasBgm = true;
    m_bgmPlayer->setSource(url);
    m_bgmPlayer->play();
}

void BGMController::stopBgm()
{
    m_hasBgm = false;
    m_bgmPlayer->stop();
}

void BGMController::playEffect(const QString& name)
{
    if (m_muted) {
        return;
    }
    const QUrl url = toPlayableUrl(resolveEffect(name));
    if (url.isEmpty()) {
        return;
    }
    // 音效每次从头播：先 stop 再设源，重叠调用时新的语音覆盖旧的
    m_effectPlayer->stop();
    m_effectPlayer->setSource(url);
    m_effectPlayer->play();
}

void BGMController::setMuted(bool muted)
{
    if (m_muted == muted) {
        return;
    }
    m_muted = muted;
    applyAudioState();
    emit mutedChanged(m_muted);
}

void BGMController::setVolume(qreal volume)
{
    m_volume = qBound<qreal>(0.0, volume, 1.0);
    applyAudioState();
}

void BGMController::applyAudioState()
{
    m_bgmOutput->setMuted(m_muted);
    m_effectOutput->setMuted(m_muted);
    m_bgmOutput->setVolume(m_volume);
    m_effectOutput->setVolume(m_volume);
}

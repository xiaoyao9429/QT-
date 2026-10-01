#ifndef BGMCONTROLLER_H
#define BGMCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>

class QAudioOutput;
class QMediaPlayer;

// 背景音乐与音效控制器（全局单例）
// 资源清单来自 :/conf/playList.json，按分组名索引
class BGMController : public QObject
{
    Q_OBJECT

public:
    // 背景音乐场景，对应 playList.json 的 BGM / Ending 分组
    enum class Scene {
        Welcome,    // 欢迎界面
        Normal,     // 正常对局
        Exciting,   // 紧张对局（如剩牌少、有炸弹）
        Win,        // 胜利结算
        Lose        // 失败结算
    };
    Q_ENUM(Scene)

    // 全局唯一实例：BGMController::instance()->playBgm(Scene::Normal)
    static BGMController* instance();

    // ===== 背景音乐：循环播放，同一场景重复调用会被忽略 =====
    void playBgm(Scene scene);
    void stopBgm();

    // ===== 音效：与背景音乐互不干扰，可叠播 =====
    // name 为资源文件名（不含扩展名），如 "Special_Bomb"、"Man_zhadan"、"Woman_buyao1"
    void playEffect(const QString& name);

    // ===== 音量与静音（同时作用于背景音乐和音效）=====
    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }
    void setVolume(qreal volume);     // 0.0 ~ 1.0
    qreal volume() const { return m_volume; }

    // 当前背景音乐场景，无播放时为 nullptr 语义（用 m_hasBgm 标记）
    bool hasBgm() const { return m_hasBgm; }
    Scene currentScene() const { return m_currentScene; }

signals:
    void mutedChanged(bool muted);

private:
    explicit BGMController(QObject* parent = nullptr);
    ~BGMController() override;
    Q_DISABLE_COPY_MOVE(BGMController)

    // 读取 playList.json 到 m_groups
    void loadPlayList();
    // 场景 -> 资源路径
    QString resolveScene(Scene scene) const;
    // 音效名 -> 资源路径（找不到则按 :/music/<name>.mp3 兜底）
    QString resolveEffect(const QString& name) const;
    // 把 json 里的 "qrc:/music/x.mp3" 归一化成 Qt 资源路径 ":/music/x.mp3"
    static QString toResourcePath(const QString& path);
    // 资源路径 -> 可直接播放的 URL。Qt6 的 FFmpeg 后端不支持直接读 qrc，
    // 这里把用到的音频按需释放到临时目录，再返回 file:// URL（结果会缓存）
    QUrl toPlayableUrl(const QString& resPath);
    // 应用当前音量/静音设置到两路输出
    void applyAudioState();

    QMediaPlayer* m_bgmPlayer = nullptr;
    QAudioOutput* m_bgmOutput = nullptr;
    QMediaPlayer* m_effectPlayer = nullptr;
    QAudioOutput* m_effectOutput = nullptr;

    QHash<QString, QStringList> m_groups;   // 分组名 -> 资源路径列表
    QHash<QString, QString> m_pathIndex;    // 文件名（不含扩展名）-> 资源路径
    QHash<QString, QString> m_extracted;    // 资源路径 -> 临时文件绝对路径
    QString m_audioTempDir;                 // 音频释放目录
    bool m_muted = false;
    qreal m_volume = 0.6;
    bool m_hasBgm = false;
    Scene m_currentScene = Scene::Welcome;
};

#endif // BGMCONTROLLER_H

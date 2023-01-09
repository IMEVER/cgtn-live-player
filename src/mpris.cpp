/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp mpris.xml -i src/mediaview.h -l MediaView -a mpris.h
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "mpris.h"
/*
 * Implementation of adaptor class MediaPlayer2Adaptor
 */

MediaPlayer2Adaptor::MediaPlayer2Adaptor(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

MediaPlayer2Adaptor::~MediaPlayer2Adaptor()
{
    // destructor
}

bool MediaPlayer2Adaptor::canQuit() const
{
    // get the value of property CanQuit
    return false;
}

bool MediaPlayer2Adaptor::canRaise() const
{
    // get the value of property CanRaise
    return true;
}

bool MediaPlayer2Adaptor::canSetFullscreen() const
{
    // get the value of property CanSetFullscreen
    return false; //qvariant_cast< bool >(parent()->property("CanSetFullscreen"));
}

QString MediaPlayer2Adaptor::desktopEntry() const
{
    // get the value of property DesktopEntry
    return "cgtn-live-player";
}

bool MediaPlayer2Adaptor::fullscreen() const
{
    // get the value of property Fullscreen
    return false; //qvariant_cast< bool >(parent()->property("Fullscreen"));
}

void MediaPlayer2Adaptor::setFullscreen(bool value)
{
    // set the value of property Fullscreen
    // parent()->setProperty("Fullscreen", QVariant::fromValue(value));
}

bool MediaPlayer2Adaptor::hasTrackList() const
{
    // get the value of property HasTrackList
    return false; //qvariant_cast< bool >(parent()->property("HasTrackList"));
}

QString MediaPlayer2Adaptor::identity() const
{
    // get the value of property Identity
    return "央视外语频道";
}

QStringList MediaPlayer2Adaptor::supportedMimeTypes() const
{
    // get the value of property SupportedMimeTypes
    return QStringList(); //qvariant_cast< QStringList >(parent()->property("SupportedMimeTypes"));
}

QStringList MediaPlayer2Adaptor::supportedUriSchemes() const
{
    // get the value of property SupportedUriSchemes
    return QStringList(); //qvariant_cast< QStringList >(parent()->property("SupportedUriSchemes"));
}

void MediaPlayer2Adaptor::Quit()
{
    // handle method call org.mpris.MediaPlayer2.Quit
    // parent()->quitApp();
}

void MediaPlayer2Adaptor::Raise()
{
    // handle method call org.mpris.MediaPlayer2.Raise
    parent()->raiseApp();
}

/*
 * Implementation of adaptor class PlayerAdaptor
 */

PlayerAdaptor::PlayerAdaptor(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

PlayerAdaptor::~PlayerAdaptor()
{
    // destructor
}

bool PlayerAdaptor::canControl() const
{
    // get the value of property CanControl
    return true; //qvariant_cast< bool >(parent()->property("CanControl"));
}

bool PlayerAdaptor::canGoNext() const
{
    // get the value of property CanGoNext
    return false; //qvariant_cast< bool >(parent()->property("CanGoNext"));
}

bool PlayerAdaptor::canGoPrevious() const
{
    // get the value of property CanGoPrevious
    return false; //qvariant_cast< bool >(parent()->property("CanGoPrevious"));
}

bool PlayerAdaptor::canPause() const
{
    // get the value of property CanPause
    return true ;//qvariant_cast< bool >(parent()->property("CanPause"));
}

bool PlayerAdaptor::canPlay() const
{
    // get the value of property CanPlay
    return true; //qvariant_cast< bool >(parent()->property("CanPlay"));
}

bool PlayerAdaptor::canSeek() const
{
    // get the value of property CanSeek
    return false; //qvariant_cast< bool >(parent()->property("CanSeek"));
}

QString PlayerAdaptor::loopStatus() const
{
    // get the value of property LoopStatus
    return "playlist"; //qvariant_cast< QString >(parent()->property("LoopStatus"));
}

void PlayerAdaptor::setLoopStatus(const QString &value)
{
    // set the value of property LoopStatus
    // parent()->setProperty("LoopStatus", QVariant::fromValue(value));
}

double PlayerAdaptor::maximumRate() const
{
    // get the value of property MaximumRate
    return 1; //qvariant_cast< double >(parent()->property("MaximumRate"));
}

QVariantMap PlayerAdaptor::metadata() const
{
    // get the value of property Metadata
    return parent()->metaData(); //qvariant_cast< QVariantMap >(parent()->property("Metadata"));
}

double PlayerAdaptor::minimumRate() const
{
    // get the value of property MinimumRate
    return 1; //qvariant_cast< double >(parent()->property("MinimumRate"));
}

QString PlayerAdaptor::playbackStatus() const
{
    // get the value of property PlaybackStatus
    return parent()->playbackStatus(); //qvariant_cast< QString >(parent()->property("PlaybackStatus"));
}

qlonglong PlayerAdaptor::position() const
{
    // get the value of property Position
    return 0; //qvariant_cast< qlonglong >(parent()->property("Position"));
}

double PlayerAdaptor::rate() const
{
    // get the value of property Rate
    return 1; //qvariant_cast< double >(parent()->property("Rate"));
}

void PlayerAdaptor::setRate(double value)
{
    // set the value of property Rate
    // parent()->setProperty("Rate", QVariant::fromValue(value));
}

bool PlayerAdaptor::shuffle() const
{
    // get the value of property Shuffle
    return false; //qvariant_cast< bool >(parent()->property("Shuffle"));
}

void PlayerAdaptor::setShuffle(bool value)
{
    // set the value of property Shuffle
    //parent()->setProperty("Shuffle", QVariant::fromValue(value));
}

double PlayerAdaptor::volume() const
{
    // get the value of property Volume
    return 0; //qvariant_cast< double >(parent()->property("Volume"));
}

void PlayerAdaptor::setVolume(double value)
{
    // set the value of property Volume
    //parent()->setProperty("Volume", QVariant::fromValue(value));
}

void PlayerAdaptor::Next()
{
    // handle method call org.mpris.MediaPlayer2.Player.Next
    // parent()->skip();
}

void PlayerAdaptor::OpenUri(const QString &Uri)
{
    // handle method call org.mpris.MediaPlayer2.Player.OpenUri
    //parent()->OpenUri(Uri);
}

void PlayerAdaptor::Pause()
{
    // handle method call org.mpris.MediaPlayer2.Player.Pause
    parent()->pause();
}

void PlayerAdaptor::Play()
{
    // handle method call org.mpris.MediaPlayer2.Player.Play
    parent()->pause();
}

void PlayerAdaptor::PlayPause()
{
    // handle method call org.mpris.MediaPlayer2.Player.PlayPause
    parent()->pause();
}

void PlayerAdaptor::Previous()
{
    // handle method call org.mpris.MediaPlayer2.Player.Previous
    // parent()->skipBackward();
}

void PlayerAdaptor::Seek(qlonglong Offset)
{
    // handle method call org.mpris.MediaPlayer2.Player.Seek
//    parent()->Seek(Offset);
}

void PlayerAdaptor::SetPosition(const QDBusObjectPath &TrackId, qlonglong Position)
{
    // handle method call org.mpris.MediaPlayer2.Player.SetPosition
    // parent()->SetPosition(TrackId, Position);
}

void PlayerAdaptor::Stop()
{
    // handle method call org.mpris.MediaPlayer2.Player.Stop
    parent()->pause();
}


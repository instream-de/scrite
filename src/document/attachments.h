/****************************************************************************
**
** Copyright (C) TERIFLIX Entertainment Spaces Pvt. Ltd. Bengaluru
** Author: Prashanth N Udupa (prashanth.udupa@teriflix.com)
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H

#include "qobjectproperty.h"
#include "qobjectserializer.h"
#include "objectlistpropertymodel.h"

#include <QFileInfo>
#include <QMimeType>
#include <QQuickItem>
#include <QQmlEngine>

class Attachment : public QObject, public QObjectSerializer::Interface
{
    Q_OBJECT
    Q_INTERFACES(QObjectSerializer::Interface)
    QML_ELEMENT
    QML_UNCREATABLE("Instantiation from QML not allowed.")

public:
    Attachment(QObject *parent = nullptr);
    ~Attachment();
    Q_SIGNAL void aboutToDelete(Attachment *ptr);

    enum Type { Photo, Video, Audio, Document };
    Q_ENUM(Type)
    Q_PROPERTY(Type type READ type NOTIFY typeChanged)
    Type type() const { return m_type; }
    Q_SIGNAL void typeChanged();

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    void setTitle(const QString &val);
    QString title() const { return m_title; }
    Q_SIGNAL void titleChanged();

    Q_PROPERTY(QString originalFileName READ originalFileName NOTIFY originalFileNameChanged)
    QString originalFileName() const { return m_originalFileName; }
    Q_SIGNAL void originalFileNameChanged();

    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    QString filePath() const { return m_filePath; }
    Q_SIGNAL void filePathChanged();

    Q_PROPERTY(QString mimeType READ mimeType NOTIFY mimeTypeChanged)
    QString mimeType() const { return m_mimeType; }
    Q_SIGNAL void mimeTypeChanged();

    Q_INVOKABLE void openAttachmentAnonymously();
    Q_INVOKABLE void openAttachmentInPlace();

    Q_SIGNAL void attachmentModified();

    bool isValid() const;

    bool isRemoveFileOnDelete() const { return m_removeFileOnDelete; }

    // QObjectSerializer::Interface interface
    void serializeToJson(QJsonObject &) const;
    void deserializeFromJson(const QJsonObject &);

    static Type determineType(const QFileInfo &fi);

private:
    friend class Attachments;
    friend class AttachmentsDropArea;
    void setType(Type val);
    void setFilePath(const QString &val);
    void setMimeType(const QString &val);
    void setOriginalFileName(const QString &val);
    bool removeAttachedFile();
    void onDfsAuction(const QString &filePath, int *claims);
    void setRemoveFileOnDelete(bool val) { m_removeFileOnDelete = val; }

private:
    QString m_name;
    QString m_title;
    QString m_filePath;
    QString m_mimeType;
    QString m_originalFileName;
    Type m_type = Document;
    QString m_anonFilePath;
    bool m_removeFileOnDelete = false;
};

class Attachments : public ObjectListPropertyModel<Attachment *>,
                    public QObjectSerializer::Interface
{
    Q_OBJECT
    Q_INTERFACES(QObjectSerializer::Interface)
    QML_ELEMENT
    QML_UNCREATABLE("Instantiation from QML not allowed.")

public:
    Attachments(QObject *parent = nullptr);
    ~Attachments();

    enum AllowedType {
        PhotosOnly = 1,
        VideosOnly = 2,
        AudioOnly = 4,
        MediaOnly = 7,
        NoMedia = 8,
        DocumentsOfAnyType = 15
    };
    Q_ENUM(AllowedType)
    Q_PROPERTY(AllowedType allowedType READ allowedType NOTIFY allowedTypeChanged STORED false)
    void setAllowedType(AllowedType val); // Can only be called from C++ classes.
    AllowedType allowedType() const { return m_allowedType; }
    Q_SIGNAL void allowedTypeChanged();

    static bool canAllow(const QFileInfo &fi, AllowedType allowed);
    static QMimeType mimeTypeFor(const QFileInfo &fi);

    Q_PROPERTY(QStringList nameFilters READ nameFilters NOTIFY allowedTypeChanged STORED false)
    QStringList nameFilters() const { return m_nameFilters; }
    Q_SIGNAL void nameFiltersChanged();

    Q_INVOKABLE Attachment *includeAttachmentFromFileUrl(const QUrl &fileUrl)
    {
        if (fileUrl.isLocalFile())
            return this->includeAttachment(fileUrl.toLocalFile());
        return nullptr;
    }
    Q_INVOKABLE Attachment *includeAttachment(const QString &filePath);
    Q_INVOKABLE void removeAttachment(Attachment *ptr);
    Q_INVOKABLE Attachment *attachmentAt(int index) const;
    Q_INVOKABLE Attachment *firstAttachment() const { return this->attachmentAt(0); }
    Q_INVOKABLE Attachment *lastAttachment() const
    {
        return this->attachmentAt(this->attachmentCount() - 1);
    }
    Q_INVOKABLE void removeAllAttachments();

    Q_PROPERTY(int attachmentCount READ attachmentCount NOTIFY attachmentCountChanged)
    int attachmentCount() const { return this->size(); }
    Q_SIGNAL void attachmentCountChanged();

    Q_SIGNAL void attachmentsModified();

    // QObjectSerializer::Interface interface
    void serializeToJson(QJsonObject &json) const;
    void deserializeFromJson(const QJsonObject &json);

private:
    void includeAttachment(Attachment *ptr);
    void attachmentDestroyed(Attachment *ptr);
    void includeAttachments(const QList<Attachment *> &list);

private:
    AllowedType m_allowedType = DocumentsOfAnyType;
    QStringList m_nameFilters;
};

class AttachmentsDropArea : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    AttachmentsDropArea(QQuickItem *parent = nullptr);
    ~AttachmentsDropArea();

    Q_PROPERTY(Attachments* target READ target WRITE setTarget NOTIFY targetChanged)
    void setTarget(Attachments *val);
    Attachments *target() const { return m_target; }
    Q_SIGNAL void targetChanged();

    Q_PROPERTY(int allowedType READ allowedType WRITE setAllowedType NOTIFY allowedTypeChanged)
    void setAllowedType(int val);
    int allowedType() const { return m_allowedType; }
    Q_SIGNAL void allowedTypeChanged();

    Q_PROPERTY(QStringList allowedExtensions READ allowedExtensions WRITE setAllowedExtensions NOTIFY allowedExtensionsChanged)
    void setAllowedExtensions(const QStringList &val);
    QStringList allowedExtensions() const { return m_allowedExtensions; }
    Q_SIGNAL void allowedExtensionsChanged();

    Q_PROPERTY(bool active READ isActive NOTIFY attachmentChanged)
    bool isActive() const { return m_attachment != nullptr; }

    Q_PROPERTY(Attachment* attachment READ attachment NOTIFY attachmentChanged)
    Attachment *attachment() const { return m_attachment; }
    Q_SIGNAL void attachmentChanged();

    Q_PROPERTY(QPointF mouse READ mouse NOTIFY mouseChanged)
    QPointF mouse() const { return m_mouse; }
    Q_SIGNAL void mouseChanged();

    Q_INVOKABLE void allowDrop();
    Q_INVOKABLE void denyDrop();

signals:
    void dropped();

protected:
    // QQuickItem interface
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);

private:
    void raiseWindow();
    void resetAttachments();
    void setAttachment(Attachment *val);
    void setMouse(const QPointF &val);
    bool prepareAttachmentFromMimeData(const QMimeData *mimeData);

private:
    bool m_active = false;
    QPointF m_mouse;
    bool m_allowDrop = true;
    int m_allowedType = 0;
    QStringList m_allowedExtensions;
    Attachment *m_attachment = nullptr;
    Attachments *m_target = nullptr;
    QPointer<QTimer> m_raiseWindowTimer;
};

#endif // ATTACHMENTS_H

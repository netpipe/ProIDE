#ifdef DBUS
#include        <iostream>
#include        <QtCore/QThread>
#include        <QtCore/QtCore>
#include        <QtDBus/QDBusInterface>

//"org.freedesktop.PowerManagement",  // from list on left
//                     "/org/freedesktop/PowerManagement", // from first line of screenshot
//                     "org.freedesktop.PowerManagement",  // from above Methods
#define         DBUS_PATH       "org.qtcoin.DBus"
#define         DBUS_INTERFACE  "org/qtcoin/message"
#define         DBUS_NAME       "org.qtcoin.message"

class           DBusHandler : public QThread
{
  Q_OBJECT

private:

  void          run(void)
  {

  }

public:
  DBusHandler(void) {

      QDBusConnection connection = QDBusConnection::sessionBus();

      connection.registerObject("/testObject", this);

      connection.registerService("dbustester.test");



     // QDBusInterface iface( DBUS_PATH, DBUS_INTERFACE, DBUS_NAME, QDBusConnection::sessionBus(), 0 );

    //  QDBusConnection connection = QDBusConnection::sessionBus();


    //  connection.registerService("my.qdbus.example");
    //  connection.registerObject("/", this, QDBusConnection::ExportAllSlots);

    //  connection.registerService(DBUS_PATH);
    //  connection.registerObject(DBUS_PATH, this, QDBusConnection::ExportAllSlots);
    //  connection.connect(QString(), DBUS_PATH, DBUS_INTERFACE, DBUS_NAME, this, SLOT(remoteCall(QString)));

    //  exec();



  }
  virtual ~DBusHandler(void) {}

  void          stop(void)
  {
    QDBusConnection connection = QDBusConnection::sessionBus();

    connection.unregisterObject(DBUS_PATH);
    connection.unregisterService(QString());
    connection.disconnectFromBus(connection.name());
    QThread::quit();
  }

public slots:
  void          remoteCall(QByteArray message)
  {
    std::cout << "Message size: "  << message.size() << std::endl;
  }

  void          remoteCall(QString message) {
    std::cout << "Message size: "  << message.size() << " data: " << message.toUtf8().constData() << std::endl;
  }

};
#endif

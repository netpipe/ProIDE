//https://gist.github.com/ben-dua/5c92b8b6e32aebe31054
#include        "DBusHandler.hpp"

int             main(int ac, char **av)
{
  QCoreApplication      app(ac, av);
  DBusHandler           handler;

  handler.moveToThread(&handler);

  handler.start();
  while (not handler.isRunning());

  //  app.exec();
  sleep(10); // This give me time to type the command line: "qdbus my.qdbus.example / local.DBusHandler.remoteCall a_message"

  handler.stop();
  while (handler.isRunning());
}

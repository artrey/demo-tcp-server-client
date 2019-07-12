using System;

namespace DemoTcpServer
{
    class Program
    {
        static void Main(string[] args)
        {
            var port = 8080;
            if (args.Length > 0)
                int.TryParse(args[0], out port);
            var server = new TcpServer(port);
            server.Start();
            while (Console.ReadLine() != "exit") ;
            server.Stop();
        }
    }
}

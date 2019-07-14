using System;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace DemoTcpServer
{
    public class TcpServer
    {
        private readonly TcpListener _listener;
        private volatile bool _started = false;
        private Thread _bg;

        public TcpServer(int port)
        {
            IPAddress localAddr = IPAddress.Parse("0.0.0.0");
            _listener = new TcpListener(localAddr, port);
        }

        public void Start()
        {
            // запуск слушателя
            _listener.Start();

            _started = true;
            _bg = new Thread(Logic) { IsBackground = true };
            _bg.Start();
        }

        public void Stop()
        {
            _started = false;
            if (!_bg.Join(1500))
            {
                _bg.Abort();
            }
            _listener.Stop();
        }

        private void Logic()
        {
            while (_started)
            {
                Console.WriteLine($@"{DateTime.Now} | Wait connection... ");

                // получаем входящее подключение
                TcpClient client = _listener.AcceptTcpClient();
                Console.WriteLine($@"{DateTime.Now} | Client connected...");
                var ips = (client.Client.RemoteEndPoint as IPEndPoint).Address.ToString()
                    .Split('.').Select(s => byte.Parse(s)).ToArray();

                // получаем сетевой поток для чтения и записи
                NetworkStream stream = client.GetStream();

                while (_started && client.Connected)
                {
                    try
                    {
                        // преобразуем сообщение в массив байтов
                        byte[] data = new byte[] { 0x3f, 0xe4, ips[0], ips[1], ips[2], ips[3] };
                        // отправка сообщения
                        stream.Write(data, 0, data.Length);

                        Thread.Sleep(200);

                        var dt = DateTime.Now;
                        data = new byte[] { (byte)dt.Hour, (byte)dt.Minute, (byte)dt.Second, 0xf3 };
                        // отправка сообщения
                        stream.Write(data, 0, data.Length);
                    }
                    catch (Exception) { }

                    Thread.Sleep(800);
                }

                try
                {
                    // закрываем поток
                    stream.Close();
                    // закрываем подключение
                    client.Close();
                }
                catch (Exception) { }
            }
        }
    }
}

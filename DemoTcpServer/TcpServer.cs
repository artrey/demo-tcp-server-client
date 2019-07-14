using System;
using System.Linq;
using System.Net;
using System.Net.Sockets;
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
                var random = new Random((int)(DateTime.Now.Ticks % int.MaxValue));

                // получаем сетевой поток для чтения и записи
                NetworkStream stream = client.GetStream();

                while (_started && client.Connected)
                {
                    try
                    {
                        var m = new Message();
                        var dt = DateTime.Now;
                        var count = random.Next() % 3;
                        m.AddData(ips[0]).AddData(ips[1]).AddData(ips[2]).AddData(ips[3]);
                        m.AddData((byte)dt.Hour).AddData((byte)dt.Minute).AddData((byte)dt.Second);
                        for (int i = 0; i < count; ++i)
                        {
                            m.AddData((byte)(random.Next() % 255));
                        }

                        // преобразуем сообщение в массив байтов
                        byte[] data = m.Data;
                        var size = random.Next() % (data.Length - 1);
                        // отправка сообщения
                        stream.Write(data, 0, size);

                        Thread.Sleep(200);

                        // отправка сообщения
                        stream.Write(data, size, data.Length - size);
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

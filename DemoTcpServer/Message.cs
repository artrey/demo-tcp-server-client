using System;
using System.Collections.Generic;
using System.Linq;

namespace DemoTcpServer
{
    public class Message
    {
        private LinkedList<byte> _data = new LinkedList<byte>();

        public byte[] Data
        {
            get
            {
                var data = new LinkedList<byte>(_data);
                data.AddFirst((byte)(data.Count + 3));
                data.AddFirst(0x3f);
                data.AddLast(Crc(data.ToArray()));
                return data.ToArray();
            }
        }

        public Message AddData(byte data)
        {
            _data.AddLast(data);
            return this;
        }

        public Message AddData(object data)
        {
            foreach (var b in DataToBytes(data))
                _data.AddLast(b);
            return this;
        }

        public static byte[] DataToBytes(object data)
        {
            byte[] bytes;
            var t = data.GetType();
            if (t == typeof(int))
            {
                bytes = BitConverter.GetBytes((int)data);
            }
            else
            {
                throw new NotImplementedException($@"Type '{t}' not supported");
            }
            if (BitConverter.IsLittleEndian)
                Array.Reverse(bytes);
            return bytes;
        }

        public static byte Crc(byte[] data)
        {
            return data.Aggregate((byte)0, (a, v) => { return (byte)(a ^ v); });
        }
    }
}

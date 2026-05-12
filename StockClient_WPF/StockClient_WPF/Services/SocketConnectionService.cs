using Google.Protobuf;
using StockClient_WPF.Interfaces;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Text;

namespace StockClient_WPF.Services
{
    internal class SocketConnectionService : IServerConnection<Packet, Packet>
    {
        private TcpClient? _client;
        private NetworkStream? _stream;
        private string Ip { get; set; }
        private int Port { get; set; }

        public void Init()
        {
            // TODO: 추후 환경설정 읽어오도록 변경.
            this.Ip = "127.0.0.1";
            this.Port = 4578;
        }

        public void CreateConnection()
        {
            if (Ip == null || Port == 0)
            {
                throw new InvalidProgramException();
            }

            _client = new TcpClient();
            _client.Connect(Ip, Port);
            _stream = _client.GetStream();
            Console.WriteLine("서버 연결 성공");
        }

        public void DestroyConnection()
        {
            _client?.Close();
            _stream?.Close();
        }

        public Packet Receive(int bufferSize = 1024)
        {
            if (_stream == null)
            {
                throw new InvalidProgramException();
            }

            byte[] buffer = new byte[bufferSize];
            int bytesRead = _stream.Read(buffer, 0, buffer.Length);
            byte[] result = new byte[bytesRead];

            try
            {
                return Packet.Parser.ParseFrom(result);
            }
            catch (Exception ex)
            {
                throw new DecoderFallbackException();
            }
        }

        public void Send(Packet obj)
        {
            if (_stream == null)
            {
                throw new InvalidProgramException();
            }

            int size = obj.CalculateSize();

            using var ms = new MemoryStream();
            using var cos = new CodedOutputStream(ms);
            obj.WriteTo(cos);
            cos.Flush();

            byte[] data = ms.ToArray();
            _stream.Write(data, 0, data.Length);
        }
    }
}

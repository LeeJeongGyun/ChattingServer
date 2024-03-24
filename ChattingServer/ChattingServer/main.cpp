
#include "pch.h"
#include "ChattingServer.h"
#include "CLanMonitorClient.h"
PDHMonitor g_Monitor(L"ChattingServer");
CpuUsage g_CPUTime;
ChattingServer server;

int64 sessionArr[1000];

int main()
{
    server.Start();
    ulong recvTPS;
    ulong contentTPS;
    while (true)
    {
        g_Monitor.UpdatePDHCounter();
        g_CPUTime.UpdateCpuTime();
        recvTPS = server.GetRecvTPS();
        wprintf(L"---------------------------------------------------------------------\n");
        wprintf(L"Session Count: %d\n", server.GetSessionCount());
        wprintf(L"Player Count: %d\n", server.GetPlayerCount());
        wprintf(L"Accept Total: %llu\n", server.GetAcceptTotalCnt());
        wprintf(L"Accept Thread TPS: %d\n", server.GetAcceptTPS());
        wprintf(L"Update Thread TPS: %d\n", server.GetUpdateTPS());
        wprintf(L"Update Thread JobQueue: %d\n", server.GetJobQueueSize());
        wprintf(L"Send Packet TPS: %d\n", server.GetSendTPS());
        wprintf(L"Recv Packet TPS: %d\n", recvTPS);
        wprintf(L"User Pool Count: %d\n", server.GetUserPoolCount());
        wprintf(L"PacketPool Count: %d\n", Packet::_packetPool.GetUseCount());
        wprintf(L"JobPacketPool Count: %d\n", server.GetJobPacketPoolSize());
        wprintf(L"Send KBytes Per Sec: %u KB\n", server.GetSendBytes() / 1024);

        wprintf(L"--------------------------PDH Data-----------------------------------\n");
        wprintf(L"Nonpaged Pool Usage: %.2fMB\n", g_Monitor.GetNonpagedPoolUsage() / 1024 / 1024);
        wprintf(L"Process User Memory Usage: %.2fMB\n", g_Monitor.GetProcessUserMemoryUsage() / 1024 / 1024);
        wprintf(L"Process Virtual Memory Size: %.2fMB\n", g_Monitor.GetVirtualMemorySize() / 1024 / 1024);
        wprintf(L"Ethernet Recv Bytes: %.2fKB\n", g_Monitor.GetEthernetRecvBytes());
        wprintf(L"Ethernet Send Bytes: %.2fKB\n", g_Monitor.GetEthernetSendBytes());
        wprintf(L"Server Available Memory: %.2fGB\n", g_Monitor.GetAvailableMemoryUsage() / 1024);

        wprintf(L"--------------------------CPU Usage----------------------------------\n");
        wprintf(L"Processor [T:%.1f%% U:%.1f%% K:%.1f%%] [Process T:%.1f%% U:%.1f%% K:%.1f%%]\n",
            g_CPUTime.GetProcessorTotal(), g_CPUTime.GetProcessorUser(), g_CPUTime.GetProcessorKernel(),
            g_CPUTime.GetProcessTotal(), g_CPUTime.GetProcessUser(), g_CPUTime.GetProcessKernel());
        wprintf(L"---------------------------------------------------------------------\n");

        wprintf(L"-------------------------Message Ratio-------------------------------\n");
        contentTPS = server._lastContentTPS;

        wprintf(L"Content Message[Login:%.1f SectorMove:%.1f Message:%.1f]\n",
            (double)server._lastContentLoginTPS / contentTPS * 100, (double)server._lastContentSectorMoveTPS / contentTPS * 100, (double)server._lastContentMessgaeTPS / contentTPS * 100);

        int32 sectorNum = 0;
        int32 ratioValue = 0;

        auto endIter = server._playerDistributeMap.end();
        wprintf(L"Player Num Per Sector\n");
        for (auto iter = server._playerDistributeMap.begin(); iter != endIter; ++iter)
        {
            if (sectorNum < 9)
            {
                int32 tempSectorNum = (int32)(ceil(9 * (iter->second / 2500.)));
                ratioValue += iter->first * tempSectorNum;
                sectorNum += tempSectorNum;

                // NOTE
                // �ؿ� ���� Ȯ��
                /*if (iter->first > n��)
                {
                    tempSectorNum = 1;
                    ratioValue += iter->first * tempSectorNum;
                    sectorNum += tempSectorNum;
                }*/
            }

            wprintf(L"[%d : %d] ", iter->first, iter->second);
        }

        // NOTE
        // ���� 500���� �ִ� ���Ͱ� 1����� 1 / 2500 �̱� ������ ������ �ϸ� ��������.
        // ex) 9 * (1 / 2500.) �ø��ص� 1�� ���� �ʱ� ������ ������ ������ �ʴ´�. 
        // �̷��� �� ���Ϳ� ���� �ο����� ������ ��쿡�� ������ ��� ������ Ȯ���� �ؾ�������
        // ���� �ο����� ���س��� �� �̻��� �Ǵ� ���Ͱ� �ִٸ� ������ �ݿ��ϵ��� �ڵ� �ۼ����ش�.

        wprintf(L"\nRecvTPS And SendTPS Ratio [1 : %d]\n", ratioValue);
        wprintf(L"---------------------------------------------------------------------\n");

        char key;
        if (_kbhit())
        {
            key = _getch();
            if (key == VK_RETURN)
                g_Profiler.ProfileDataOutText(L"CAHTTING");
        }

        Sleep(1000);
    }
}

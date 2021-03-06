#ifndef MISC_H
#define MISC_H

#include <fstream>
#include <istream>
#include <string>
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <chrono>

class FileReader
{
	int iData[100];			// Will hold information for the system to use.
	double dData[1000];		// Will hold data read from the 
	FileReader() { }
public:

	static FileReader &GetInstance()
	{
		static FileReader instance;
		return instance;
	}

	FileReader(FileReader const&) = delete;
	void operator=(FileReader const&) = delete;

	~FileReader() { }

	inline void ReadFromFile(std::string filename, unsigned int numOfLines = 0, int mode = 0)
	{
		if (numOfLines == 0)
		{
			std::cout << "[ " << filename << " ] Line number must be higher than 0.";
			return;
		}
		
		std::ifstream file(filename.c_str());
		if (file.is_open())
		{
			if (mode == 1)
				for (unsigned int i = 0; i < numOfLines; file >> std::fixed >> std::setprecision(6) >> dData[i++]);
			else 
				for (unsigned int i = 0; i < numOfLines; file >> iData[i++]);
			file.close();
			file.clear();
		}
		else
		{
			file.close();
			std::cout << "Couldn't read file!\n";
			std::cin.get();
			exit(0);
		}
	}

	inline int FetchIntData(int i) const { return iData[i]; }
	inline double FetchDoubleData(int i) const { return dData[i]; }

	inline void WriteToFile(char** map, int caveNumber, int y, int x, double time, int precision = 6)
	{
		if (CreateDirectoryA("Caves", NULL))
		{
			std::cout << "Created \"Caves\" Directory!\n";
			Sleep(100);
		}

		//double temp = time;
		std::ofstream file("caves/" + std::to_string(x) + "x" + std::to_string(y) + "_cave_" + std::to_string(caveNumber) + ".txt");
		if (file.is_open())
		{
			//file << "Time to generate: " << std::fixed << std::setprecision(precision) << temp << " ms\n";
			for (int i = 0; i < y; i++)
			{
				/*for (int j = 0; j < y; j++)
				{*/
					file << map[i]/*[j]*/;
					file << "\n";
				/*}*/
			}
			file.close();
			file.clear();
		}
		else
			std::cout << "Couldn't write to file cave_" + std::to_string(caveNumber) + ".txt\n";

		if (caveNumber == 1)
			file.open("Data/" + std::to_string(x) + "x" + std::to_string(y) + "_data.txt");
		else 
			file.open("Data/" + std::to_string(x) + "x" + std::to_string(y) + "_data.txt", std::ios_base::app);

		if (file.is_open())
		{
			file << std::fixed << std::setprecision(precision) << time << "\n";
			file.close();
			file.clear();
		}
		else
			std::cout << "Couldn't write to file " << x << "x" << x << "_data.txt\n";
	}

	inline void WriteToFile(std::string filename, std::string message = "NULL", double data = 0.0, int precision = 7)
	{
		if (CreateDirectoryA("Data", NULL))
		{
			std::cout << "Created \"Data\" Directory!\n";
			Sleep(100);
		}

		std::ofstream file("Data/" + filename, std::ios_base::app);
		if (file.is_open())
		{
			file << message << std::fixed << std::setprecision(precision) << data << "\n";
			file.close();
			file.clear();
		}
		else
			std::cout << "Couldn't write to file " << filename << "\n";
	}
};

class Calculations
{
	double			mAvgTime, mMinTime, mMaxTime;
	//ULL				mAvgCpu, mMinCpu, mMaxCpu;

	Calculations() { mAvgTime = 0.0; mMinTime = 0.0; mMaxTime = 0.0; }
public:
	~Calculations() { }
	static Calculations &GetInstance()
	{
		static Calculations instance;
		return instance;
	}

					Calculations(Calculations const&) = delete;
	void			operator=(Calculations const&) = delete;

	inline void		SetAvgTime(double x)	{ mAvgTime = x; }
	inline void		SetMinTime(double x)	{ mMinTime = x; }
	inline void		SetMaxTime(double x)	{ mMaxTime = x; }

	inline double	GetAvgTime() const		{ return mAvgTime; }
	inline double	GetMinTime() const		{ return mMinTime; }
	inline double	GetMaxTime() const		{ return mMaxTime; }

	inline void FindTime(std::string filename, int x, int y, int numOfCavesToGenerate) 
	{
		FileReader::GetInstance().ReadFromFile(filename, numOfCavesToGenerate, 1);
		
		double temp = 100000.0;
		for (int i = 0; i < numOfCavesToGenerate; i++)
		{
			if (FileReader::GetInstance().FetchDoubleData(i) < temp)
				temp = FileReader::GetInstance().FetchDoubleData(i);
		}
		FileReader::GetInstance().WriteToFile("MinTime.txt", std::to_string(x) + "x" + std::to_string(y) + " = ", temp, 8);
		///////////////////////////////////////////////////////
		temp = 0.0;
		for (int i = 0; i < numOfCavesToGenerate; i++)
		{

			if (FileReader::GetInstance().FetchDoubleData(i) > temp)
				temp = FileReader::GetInstance().FetchDoubleData(i);
		}
		FileReader::GetInstance().WriteToFile("MaxTime.txt", std::to_string(x) + "x" + std::to_string(y) + " = ", temp, 8);
		///////////////////////////////////////////////////////
		temp = 0.0;
		double total = 0.0;
		for (int i = 0; i < numOfCavesToGenerate; i++)
		{
			total += FileReader::GetInstance().FetchDoubleData(i);
		}
		total /= numOfCavesToGenerate;
		FileReader::GetInstance().WriteToFile("AvgTime.txt", std::to_string(x) + "x" + std::to_string(y) + " = ", total, 8);
	}
};

class Timer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> mBegin, mEnd;
	double				mDuration;
public:
						Timer() { mDuration = 0; }
						~Timer() { }

	inline void			StartTimer() { mBegin = std::chrono::high_resolution_clock::now(); }
	inline void			StopTimer()  { mEnd   = std::chrono::high_resolution_clock::now(); }
	inline double		GetDuration() const { return std::chrono::duration<double, std::milli>(mEnd - mBegin).count();  }
};

class CPUUsage
{
	typedef			unsigned long long ULL;
	ULL				mAvgCpu, mMinCpu, mMaxCpu;
	FILETIME		mSysIdle, mSysKernel, mSysUser,
					mProcCreation, mProcExit, mProcKernel, mProcUser,
					mPrevSysKernel, mPrevSysUser, mPrevProcKernel, mPrevProcUser;
	float			mUsage, mMin, mMax, mTemp;
	double			mCPUUsage;
	bool			mFirstRun;
public:
	CPUUsage()		{ mFirstRun = false; }
	~CPUUsage()		{ }
	inline void		SetAvgCPU(ULL x) { mAvgCpu = x; }
	inline void		SetMinCPU(ULL x) { mMinCpu = x; }
	inline void		SetMaxCPU(ULL x) { mMaxCpu = x; }

	inline ULL		GetAvgCPUTime() const { return mAvgCpu; }
	inline ULL		GetMinCPUTime() const { return mMinCpu; }
	inline ULL		GetMaxCPUTime() const { return mMaxCpu; }

	inline float	GetUsage() const { return mUsage; }

	inline ULL		SubtractTime(const FILETIME &x, const FILETIME &y)
	{
		LARGE_INTEGER	lx, ly;
		lx.LowPart = x.dwLowDateTime;
		lx.HighPart = x.dwHighDateTime;

		ly.LowPart = y.dwLowDateTime;
		ly.HighPart = y.dwHighDateTime;

		return lx.QuadPart - ly.QuadPart;
	}

	inline double GetCPUUsage(FILETIME* prevSysKernel, FILETIME* prevSysUser, 
		FILETIME* prevProcKernel, FILETIME* prevProcUser, 
		bool firstrun = false)
	{

		double nCpuCopy = mCPUUsage;
		FILETIME	sysIdle, sysKernel, sysUser,
					procCreation, procExit, procKernel, procUser;

		if (!GetSystemTimes(&sysIdle, &sysKernel, &sysUser) ||
			!GetProcessTimes(GetCurrentProcess(), &procCreation, &procExit, &procKernel, &procUser))
		{
			std::cout << "Can't get time info.\n";
			return -1.0;
		}

		if (!firstrun)
		{
			ULL sysKernelDiff = SubtractTime(sysKernel, mPrevSysKernel);
			ULL sysUserDiff = SubtractTime(sysUser, mPrevSysUser);

			ULL procKernelDiff = SubtractTime(procKernel, mPrevProcKernel);
			ULL procUserDiff = SubtractTime(procUser, mPrevProcUser);

			ULL nTotalSys = sysKernelDiff + sysUserDiff;
			ULL nTotalProc = procKernelDiff + procUserDiff;

			if (nTotalSys > 0)
			{
				mCPUUsage = (double)((100.0 * nTotalProc) / nTotalSys);
			}
		}

			//prevSysKernel->dwLowDateTime	= sysKernel.dwLowDateTime;
			//prevSysKernel->dwHighDateTime	= sysKernel.dwHighDateTime;

			//prevSysUser->dwLowDateTime		= sysUser.dwLowDateTime;
			//prevSysUser->dwHighDateTime		= sysUser.dwHighDateTime;

			//prevProcKernel->dwLowDateTime	= procKernel.dwLowDateTime;
			//prevProcKernel->dwHighDateTime	= procKernel.dwHighDateTime;

			//prevProcUser->dwLowDateTime		= procUser.dwLowDateTime;
			//prevProcUser->dwHighDateTime = procUser.dwHighDateTime;
		// }
		//	return -1.0;
		

		//ULL sysTotal		= SubtractTime(sysKernel, *prevSysKernel) + SubtractTime(sysUser, *prevSysUser);
		//ULL procTotal		= SubtractTime(procKernel, *prevProcKernel) + SubtractTime(procUser, *prevProcUser);

		//if ((double)((100.0 * procTotal) / sysTotal) > 0.00)
		//	return (double)((100.0 * procTotal) / sysTotal);
		//else return 0.0;

		mPrevSysKernel = sysKernel;
		mPrevSysUser = sysUser;
		mPrevProcKernel = procKernel;
		mPrevProcUser = procUser;

		nCpuCopy = mCPUUsage;
		return nCpuCopy;
	}

	inline void FindUsage(std::string filename, int x, int y, int numOfCavesToGenerate)
	{
		FileReader::GetInstance().ReadFromFile(filename, numOfCavesToGenerate, 1);
		double temp = 100000.0;
		for (int i = 0; i < numOfCavesToGenerate; i++)
		{
			if (FileReader::GetInstance().FetchDoubleData(i) < temp)
				temp = FileReader::GetInstance().FetchDoubleData(i);
		}
		FileReader::GetInstance().WriteToFile("MinCPUUsage.txt", std::to_string(x) + "x" + std::to_string(y) + " = ", temp, 8);
		///////////////////////////////////////////////////////
		temp = 0.0;
		for (int i = 0; i < numOfCavesToGenerate; i++)
		{
			if (FileReader::GetInstance().FetchDoubleData(i) > temp)
				temp = FileReader::GetInstance().FetchDoubleData(i);
		}
		FileReader::GetInstance().WriteToFile("MaxCPUUsage.txt", std::to_string(x) + "x" + std::to_string(y) + " = ", temp, 8);
		///////////////////////////////////////////////////////
		temp = 0.0;
		double total = 0.00000000;
		for (int i = 0; i < numOfCavesToGenerate; i++)
		{
			total += FileReader::GetInstance().FetchDoubleData(i);
		}
		total /= numOfCavesToGenerate;
		FileReader::GetInstance().WriteToFile("AvgCPUUsage.txt", std::to_string(x) + "x" + std::to_string(y) + " = ", total, 8);
	}
};


#endif
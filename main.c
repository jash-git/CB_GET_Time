
#include <time.h>
#include <stdio.h>
#include <string.h>

/* 系統起始計算年份常數定義 */
#define GRTC_DATE_SINCE_YEARS 2015
/* 每一年天數常數定義 */
#define GRTC_TIME_DAY_PER_YEARS 365
/* 每小時分鐘數常數定義 */
#define GRTC_TIME_HOURS_PER_DAY 24
/* 每小時分鐘數常數定義 */
#define GRTC_TIME_MINUTES_PER_HOUR 60
/* 每分鐘秒數常數定義 */
#define GRTC_TIME_SECONDS_PER_MINUTE 60
/* 每小時秒數常數定義 */
#define GRTC_TIME_SECONDS_PER_HOUR (GRTC_TIME_SECONDS_PER_MINUTE * GRTC_TIME_MINUTES_PER_HOUR)
/* 每一天秒數常數定義 */
#define GRTC_TIME_SECONDS_PER_DAY (GRTC_TIME_SECONDS_PER_HOUR * GRTC_TIME_HOURS_PER_DAY)

/* 時間資料結構定義宣告 */
typedef struct _GRTC_LOCAL_TIME_DATAGRAM_STRUCTURE
  {
    /* Time */
    int iSecond; /* Seconds: 0-59 */
    int iMinutes; /* Minutes: 0-59 */
    int iHours; /* Hours since midnight: 0-23 */

    /* Date */
    int iMDay; /* Day of the month: 1-31 */
    int iMonths; /* Months *since* january: 0-11 */
    int iYears; /* Years *since* 2015 */
  } GRTCLOCALTIME, *LPGRTCLOCALTIME;

/* 時間資料宣告 */
static GRTCLOCALTIME g_gltGRTCLocalTime;

/* 一年中每經過一個月所累積之天數列表 */
static const int g_iGRTCMonthElapsedDayList[13] =
  {
    /*-----------------------------------------*/
    /* 列表 */
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365,
  };

/*---------------------------------------------------------------------------*/
/* 取得系統時間 (2015/01/01-00:00:00 起至當下總秒數) */
long GRTCGetTime(GRTCLOCALTIME *lpLocalTime)
  {
    int iMDay, iYears, iMonths, iHours, iMinutes, iSecond;
    long lTotalDays, lTotalSecond;

    /* 如果沒有指定時間資料結構 */
    if(lpLocalTime == NULL)
      {
        struct tm *lpSystemDatetime;
        time_t stmiSystemSeconds;

        /* 直接由系統取得當前時間 */
        stmiSystemSeconds = time(NULL);
        lpSystemDatetime = localtime(&stmiSystemSeconds);

        /* 修正數據，並指派到計算用變數 */
        iYears = lpSystemDatetime->tm_year;
        iMonths = lpSystemDatetime->tm_mon;
        iMDay = lpSystemDatetime->tm_mday;
        iHours = lpSystemDatetime->tm_hour;
        iMinutes = lpSystemDatetime->tm_min;
        iSecond = lpSystemDatetime->tm_sec;
      }
    /* 指定了時間資料結構 */
    else
      {
        /* 修正數據，並指派到計算用變數 */
        iYears = lpLocalTime->iYears;
        iMonths = lpLocalTime->iMonths;
        iMDay = lpLocalTime->iMDay;
        iHours = lpLocalTime->iHours;
        iMinutes = lpLocalTime->iMinutes;
        iSecond = lpLocalTime->iSecond;
      }

    /* 計算總天數 */
    lTotalDays = (long)(g_iGRTCMonthElapsedDayList[iMonths] + (iMDay - 1) + (iYears * GRTC_TIME_DAY_PER_YEARS));

    /* finally seconds */
    lTotalSecond = (((((lTotalDays * GRTC_TIME_HOURS_PER_DAY) + iHours) * GRTC_TIME_MINUTES_PER_HOUR) + iMinutes) * GRTC_TIME_SECONDS_PER_MINUTE) + iSecond;

    return lTotalSecond;
  }

/*---------------------------------------------------------------------------*/
/* 取得本地時間 */
GRTCLOCALTIME *GRTCGetLocalTime(long *lpSeconds)
  {
    long lTotalSeconds, lSecondRemainder;
    long lDays, lYears, lMonths;

    /* 如果沒有指定時間參數 */
    if(lpSeconds == NULL)
      {
        /* 直接取當前系統時間進行處理 */
        lTotalSeconds = GRTCGetTime(NULL);
      }
    else
      {
        lTotalSeconds = *lpSeconds;
      }

    /* 計算並修正天數值 */
    lDays = (lTotalSeconds / GRTC_TIME_SECONDS_PER_DAY);
    lSecondRemainder = (lTotalSeconds % GRTC_TIME_SECONDS_PER_DAY);
    while(lSecondRemainder < 0)
      {
        lSecondRemainder += GRTC_TIME_SECONDS_PER_DAY;
        lDays --;
      }

    while(lSecondRemainder >= GRTC_TIME_SECONDS_PER_DAY)
      {
        lSecondRemainder -= GRTC_TIME_SECONDS_PER_DAY;
        lDays ++;
      }

    /* 產生時、分、秒欄位 */
    g_gltGRTCLocalTime.iHours = (lSecondRemainder / GRTC_TIME_SECONDS_PER_HOUR);
    lSecondRemainder %= GRTC_TIME_SECONDS_PER_HOUR;
    g_gltGRTCLocalTime.iMinutes = (lSecondRemainder / GRTC_TIME_SECONDS_PER_MINUTE);
    g_gltGRTCLocalTime.iSecond = (lSecondRemainder % GRTC_TIME_SECONDS_PER_MINUTE);

    /* 年月日計算 */
    lYears = GRTC_DATE_SINCE_YEARS;
    while((lDays < 0) || (lDays >= GRTC_TIME_DAY_PER_YEARS))
      {
        /* 訂定一年是 GRTC_TIME_DAY_PER_YEARS 天數 */
        long lYearsGuess = lYears + ((lDays / GRTC_TIME_DAY_PER_YEARS) - ((lDays % GRTC_TIME_DAY_PER_YEARS) < 0));

        /* 修正天數 */
        lDays -= ((lYearsGuess - lYears) * GRTC_TIME_DAY_PER_YEARS);
        lYears = lYearsGuess;
      }

    /* 產生年份欄位 */
    g_gltGRTCLocalTime.iYears = (lYears - GRTC_DATE_SINCE_YEARS);

    /* 查表搜尋並計算月份與日期 */
    lMonths = lYears;
    for(lMonths = 11; lDays < g_iGRTCMonthElapsedDayList[lMonths]; lMonths --) continue;
    lDays -= g_iGRTCMonthElapsedDayList[lMonths];

    /* 產生月份與日期欄位 */
    g_gltGRTCLocalTime.iMonths = lMonths;
    g_gltGRTCLocalTime.iMDay = (lDays + 1);

    return &g_gltGRTCLocalTime;
  }

int main(void)
  {
    GRTCLOCALTIME gltLocalTime, *lpLocalTime = NULL;
    long lTotalSecond;

    memset(&gltLocalTime, 0, sizeof(GRTCLOCALTIME));
    gltLocalTime.iYears = 0;
    gltLocalTime.iMonths = 9;
    gltLocalTime.iMDay = 14;

    lTotalSecond = GRTCGetTime(&gltLocalTime);//寫入 日期變秒 ->/60->分
    printf("lTotalSecond = %ld\n", lTotalSecond);

    lTotalSecond=123456*60;
    lpLocalTime = GRTCGetLocalTime(&lTotalSecond);//讀取 分鐘*60->日期
    printf("iYears = %d, iMonths = %d, iMDay = %d\n", lpLocalTime->iYears, lpLocalTime->iMonths, lpLocalTime->iMDay);

    return 0;
  }


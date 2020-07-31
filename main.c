
#include <time.h>
#include <stdio.h>
#include <string.h>

/* �t�ΰ_�l�p��~���`�Ʃw�q */
#define GRTC_DATE_SINCE_YEARS 2015
/* �C�@�~�ѼƱ`�Ʃw�q */
#define GRTC_TIME_DAY_PER_YEARS 365
/* �C�p�ɤ����Ʊ`�Ʃw�q */
#define GRTC_TIME_HOURS_PER_DAY 24
/* �C�p�ɤ����Ʊ`�Ʃw�q */
#define GRTC_TIME_MINUTES_PER_HOUR 60
/* �C������Ʊ`�Ʃw�q */
#define GRTC_TIME_SECONDS_PER_MINUTE 60
/* �C�p�ɬ�Ʊ`�Ʃw�q */
#define GRTC_TIME_SECONDS_PER_HOUR (GRTC_TIME_SECONDS_PER_MINUTE * GRTC_TIME_MINUTES_PER_HOUR)
/* �C�@�Ѭ�Ʊ`�Ʃw�q */
#define GRTC_TIME_SECONDS_PER_DAY (GRTC_TIME_SECONDS_PER_HOUR * GRTC_TIME_HOURS_PER_DAY)

/* �ɶ���Ƶ��c�w�q�ŧi */
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

/* �ɶ���ƫŧi */
static GRTCLOCALTIME g_gltGRTCLocalTime;

/* �@�~���C�g�L�@�Ӥ�Ҳֿn���ѼƦC�� */
static const int g_iGRTCMonthElapsedDayList[13] =
  {
    /*-----------------------------------------*/
    /* �C�� */
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365,
  };

/*---------------------------------------------------------------------------*/
/* ���o�t�ήɶ� (2015/01/01-00:00:00 �_�ܷ�U�`���) */
long GRTCGetTime(GRTCLOCALTIME *lpLocalTime)
  {
    int iMDay, iYears, iMonths, iHours, iMinutes, iSecond;
    long lTotalDays, lTotalSecond;

    /* �p�G�S�����w�ɶ���Ƶ��c */
    if(lpLocalTime == NULL)
      {
        struct tm *lpSystemDatetime;
        time_t stmiSystemSeconds;

        /* �����Ѩt�Ψ��o��e�ɶ� */
        stmiSystemSeconds = time(NULL);
        lpSystemDatetime = localtime(&stmiSystemSeconds);

        /* �ץ��ƾڡA�ë�����p����ܼ� */
        iYears = lpSystemDatetime->tm_year;
        iMonths = lpSystemDatetime->tm_mon;
        iMDay = lpSystemDatetime->tm_mday;
        iHours = lpSystemDatetime->tm_hour;
        iMinutes = lpSystemDatetime->tm_min;
        iSecond = lpSystemDatetime->tm_sec;
      }
    /* ���w�F�ɶ���Ƶ��c */
    else
      {
        /* �ץ��ƾڡA�ë�����p����ܼ� */
        iYears = lpLocalTime->iYears;
        iMonths = lpLocalTime->iMonths;
        iMDay = lpLocalTime->iMDay;
        iHours = lpLocalTime->iHours;
        iMinutes = lpLocalTime->iMinutes;
        iSecond = lpLocalTime->iSecond;
      }

    /* �p���`�Ѽ� */
    lTotalDays = (long)(g_iGRTCMonthElapsedDayList[iMonths] + (iMDay - 1) + (iYears * GRTC_TIME_DAY_PER_YEARS));

    /* finally seconds */
    lTotalSecond = (((((lTotalDays * GRTC_TIME_HOURS_PER_DAY) + iHours) * GRTC_TIME_MINUTES_PER_HOUR) + iMinutes) * GRTC_TIME_SECONDS_PER_MINUTE) + iSecond;

    return lTotalSecond;
  }

/*---------------------------------------------------------------------------*/
/* ���o���a�ɶ� */
GRTCLOCALTIME *GRTCGetLocalTime(long *lpSeconds)
  {
    long lTotalSeconds, lSecondRemainder;
    long lDays, lYears, lMonths;

    /* �p�G�S�����w�ɶ��Ѽ� */
    if(lpSeconds == NULL)
      {
        /* ��������e�t�ήɶ��i��B�z */
        lTotalSeconds = GRTCGetTime(NULL);
      }
    else
      {
        lTotalSeconds = *lpSeconds;
      }

    /* �p��íץ��Ѽƭ� */
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

    /* ���ͮɡB���B����� */
    g_gltGRTCLocalTime.iHours = (lSecondRemainder / GRTC_TIME_SECONDS_PER_HOUR);
    lSecondRemainder %= GRTC_TIME_SECONDS_PER_HOUR;
    g_gltGRTCLocalTime.iMinutes = (lSecondRemainder / GRTC_TIME_SECONDS_PER_MINUTE);
    g_gltGRTCLocalTime.iSecond = (lSecondRemainder % GRTC_TIME_SECONDS_PER_MINUTE);

    /* �~���p�� */
    lYears = GRTC_DATE_SINCE_YEARS;
    while((lDays < 0) || (lDays >= GRTC_TIME_DAY_PER_YEARS))
      {
        /* �q�w�@�~�O GRTC_TIME_DAY_PER_YEARS �Ѽ� */
        long lYearsGuess = lYears + ((lDays / GRTC_TIME_DAY_PER_YEARS) - ((lDays % GRTC_TIME_DAY_PER_YEARS) < 0));

        /* �ץ��Ѽ� */
        lDays -= ((lYearsGuess - lYears) * GRTC_TIME_DAY_PER_YEARS);
        lYears = lYearsGuess;
      }

    /* ���ͦ~����� */
    g_gltGRTCLocalTime.iYears = (lYears - GRTC_DATE_SINCE_YEARS);

    /* �d��j�M�íp�����P��� */
    lMonths = lYears;
    for(lMonths = 11; lDays < g_iGRTCMonthElapsedDayList[lMonths]; lMonths --) continue;
    lDays -= g_iGRTCMonthElapsedDayList[lMonths];

    /* ���ͤ���P������ */
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

    lTotalSecond = GRTCGetTime(&gltLocalTime);//�g�J ����ܬ� ->/60->��
    printf("lTotalSecond = %ld\n", lTotalSecond);

    lTotalSecond=123456*60;
    lpLocalTime = GRTCGetLocalTime(&lTotalSecond);//Ū�� ����*60->���
    printf("iYears = %d, iMonths = %d, iMDay = %d\n", lpLocalTime->iYears, lpLocalTime->iMonths, lpLocalTime->iMDay);

    return 0;
  }


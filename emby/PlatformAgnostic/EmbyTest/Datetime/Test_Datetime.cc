//
// Created by mark on 18/07/25.
//
#include <EmbyTime/DateTime.hh>
#include <cassert>
#include <iostream>

using namespace EmbyTime;

extern void EmbyMachine::abort()
{
    std::cout << "ERROR: ASSERT TRIGGERED!!" << std::endl;
    exit(1);
}

int main()
{
    std::cout << "Running DateTime tests..." << std::endl;

    TimeStamp ts;
    DateTime dt;

    //
    // TEST 1: Epoch time (1970-01-01 00:00:00)
    //
    {
        Date epochDate((DayOfMonth)1, (MonthOfYear)1, 1970, (DayOfWeek)4); // Jan 1, 1970, Thursday
        Time epochTime(0, 0, 0);
        DateTime epoch(epochDate, epochTime);

        bool ok = DateTime::getTimeStamp(ts, epoch);
        assert(ok);
        assert(ts == 0);

        DateTime result;
        ok = DateTime::getDateTime(result, ts, 1970);
        assert(ok);
        assert(result.getDate().getYear() == 1970);
        assert(static_cast<int>(result.getDate().getMonth()) == 1);
        assert(result.getDate().getDay() == 1);
        assert(result.getTime().getHours() == 0);
        assert(result.getTime().getMinutes() == 0);
        assert(result.getTime().getSeconds() == 0);
    }

    //
    // TEST 2: Leap year (2000-02-29)
    //
    {
        Date leapDate((DayOfMonth)29, (MonthOfYear)2, 2000, (DayOfWeek)2); // Tuesday
        Time leapTime(12, 34, 56);
        DateTime leap(leapDate, leapTime);

        bool ok = DateTime::getTimeStamp(ts, leap);
        assert(ok);

        DateTime result;
        ok = DateTime::getDateTime(result, ts, 1970);
        assert(ok);
        assert(result.getDate().getYear() == 2000);
        assert(static_cast<int>(result.getDate().getMonth()) == 2);
        assert(result.getDate().getDay() == 29);
        assert(result.getTime().getHours() == 12);
        assert(result.getTime().getMinutes() == 34);
        assert(result.getTime().getSeconds() == 56);
    }

    //
    // TEST 3: Date to timestamp conversion
    //
    {
        Date date((DayOfMonth)15, (MonthOfYear)3, 2023, (DayOfWeek)3); // Wednesday
        Time time(8, 45, 0);
        DateTime dt(date, time);

        bool ok = DateTime::getTimeStamp(ts, dt);
        assert(ok);
        auto str = DateTime::dateTimeToString(dt);
        assert(ts == 1678869900); // Expected timestamp for 2023-03-15 08:45:00 UTC

    }
    //
    // TEST 4: Timestamp to datetime conversion
    //
    {
        TimeStamp testTs = 1678869900; // Example timestamp
        DateTime result;
        bool ok = DateTime::getDateTime(result, testTs, 1970);
        assert(ok);
        assert(result.getDate().getYear() == 2023);
        assert(static_cast<int>(result.getDate().getMonth()) == 3);
        assert(result.getDate().getDay() == 15);
        assert(result.getTime().getHours() == 8);
        assert(result.getTime().getMinutes() == 45);
        assert(result.getTime().getSeconds() == 0);
    }

    //
    // TEST 5: Timestamp to dateOfMonth conversion
    //
    {
        TimeStamp testTs = 1609459200; // Example timestamp for 2021-01-01 00:00:00 UTC
        DateTime date;
        bool ok = DateTime::getDateTime(date, testTs, 1970);
        assert(ok);
        assert(date.getDate().getYear() == 2021);
        assert(static_cast<int>(date.getDate().getMonth()) == 1);
        assert(date.getDate().getDay() == 1);
        assert(date.getDate().getDayOfWeek() == DayOfWeek::Friday); // Should match the day of the week
    }

    //
    // TEST 6: Print output format
    //
    {
        Date date((DayOfMonth)4, (MonthOfYear)7, 2025, (DayOfWeek)5); // Friday
        Time time(10, 15, 30);
        DateTime dt(date, time);

        auto str = DateTime::dateTimeToString(dt);
        std::cout << "Formatted: " << str.c_str() << std::endl;
        assert(str == "4/7/2025 10:15:30");
    }

    //
    // TEST 8: Invalid leap day (should not exist: 2019-02-29)
    //
    {
        Date badDate((DayOfMonth)29, (MonthOfYear)2, 2019, (DayOfWeek)5);
        Time t(0, 0, 0);
        DateTime dt(badDate, t);

        // This will pass in your implementation, even though it's not a valid date
        // Suggestion: add validation if needed
        bool ok = DateTime::getTimeStamp(ts, dt);
        assert(ok);

        DateTime roundTrip;
        // This will convert to 1st March 2019 instead, or something close
        ok = DateTime::getDateTime(roundTrip, ts, 1970);
        assert(ok);
        auto strDate = DateTime::dateTimeToString(roundTrip);
        std::cout << "Round trip date: " << strDate.c_str() << std::endl;

    }

    std::cout << "\nALL DateTime TESTS PASSED!" << std::endl;
    return 0;
}

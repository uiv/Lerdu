/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/


#include <e32svr.h>
#include <StifParser.h>
#include <StifLogger.h>
#include <Stiftestinterface.h>


/* This class is used to notify the observer 
*  about the timeout 
*/
class MTimeoutObserver
{
    public: // New functions
    
    /**
    * Timeout 
    */
    virtual void HandleTimeout(TInt error) = 0;
};
    

_LIT(KFormatTimeStamp, "%:0%J%:1%T%:2%S%.%*C4%:3%+B");


/* This class is used for extra time controlling
*  The STIF timeout isn't enough
*/
class CSimpleTimeout : public CActive
{
    public:     // Enumerations
    // None

    private:    // Enumerations
    // None

    public:     // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CSimpleTimeout* NewL( MTimeoutObserver* aObserver, CStifLogger* aLogger);

    /**
    * Destructor.
    */
    ~CSimpleTimeout();

    public:     // New functions

    /**
    * Starts timeout counting
    */
    void Start(TTimeIntervalMicroSeconds aTimeout);

    /**
    * Cancel timeout counting
    */
    void Stop();


    public:     // Functions from base classes

    /**
    * RunL derived from CActive handles the completed requests.
    */
    void RunL();

    /**
    * DoCancel derived from CActive handles the cancel
    */
    void DoCancel();

    /**
    * RunError derived from CActive handles errors from active handler.
    */
    TInt RunError( TInt aError );

    protected:  // New functions
    // None

    protected:  // Functions from base classes
    // None

    private:

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL(MTimeoutObserver* aObserver, CStifLogger* aLogger);

    /**
    * C++ constructor.
    */
    CSimpleTimeout();

    public:     // Data
    // None

    protected:  // Data
    // None

    private:    // Data
    MTimeoutObserver*   iObserver;
    RTimer 						  iTimer;

    // Storage for testcase timeout.
    TTime 						iTestCaseTimeout;
    CStifLogger*				iLog;

    public:     // Friend classes
    // None

    protected:  // Friend classes
    // None

    private:    // Friend classes
    // None

};

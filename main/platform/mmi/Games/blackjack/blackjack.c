/*=============================================================================

FILE: BlackJack.c

SERVICES: BlackJack game

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 1999-2002 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/Games/src/BlackJack.c#4 $
$DateTime: 2002/11/19 16:24:08 $
  $Author: sip $
  $Change: 70105 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/15/02   sip     Replaced deprecated BREW_STATIC_APP with AEE_STATIC
06/13/02   mjv     Include customer.h instead of OEMFeatures.h
05/09/02   mjv     Ported to BREW 1.2
04/25/02   mjv     Added edit history.  Linted file.

=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif


#include "AEE.h"
#include "OEMClassIDs.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "blackjack.brh"
#include "blackjack_image.brh"
#include "AEEFile.h"
#include "err.h"
#include "appscommon.h"
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#if defined( AEE_SIMULATOR)
#define BLACKJACK_RES_FILE_LANGUAGE   ("fs:/mod/blackjack/en/"BLACKJACK_RES_FILE)
#define BLACKJACK_RES_FILE_IMAGES       ("fs:/mod/blackjack/en/"BLACKJACK_IMAGE_RES_FILE)
#else
#define BLACKJACK_RES_FILE_LANGUAGE   (AEE_RES_LANGDIR BLACKJACK_RES_FILE)
#endif

#define kNumberCardImages                      19
#define kHiddenCardTopImageIndex               18
#define kHiddenCardBottomImageIndex            17
#define kFirstCardKindImageIndex               4
#define kHandValueMax                          21
#define kDealerHitMax                          16
#define kDoubleDownLowerLimit                  9
#define kDoubleDownUpperLimit                  11
#define kHorizSpaceBetweenCards                2
#define kVerticalSpaceBetweenCards             4
#define kOverlapSpacingFactor                  (2/3)
#define kBlackJackValue                        255
#define kBlackJackAllowed                      TRUE
//#define kBlackJackNotAllowed                   FALSE
#define kDealerPlaying                         TRUE
#define kDealerNotPlaying                      FALSE
#define kDrawPlayersHand                       TRUE
#define kDontDrawPlayersHand                   FALSE
#define kDoublingDown                          TRUE
#define kNormalHit                             FALSE
#define kBetCheckAll                           TRUE
#define kBetCheckJustMinimum                   FALSE
#define kLeaveSpaceForMenu                     TRUE
#define kDontLeaveSpaceForMenu                 FALSE
#define kShowAllCards                          0
//#define kBackOfCardIndex                       52
#define kDealerPlayDelay                       1000
#define kDisplayResultsDelay                   2000
#define kShufflingMessageDelay                 6000
#define kInsuranceResultMessageDelay           5000
#define kTableMinimum                          10
#define kTableMaximum                          200
#define kInitialCash                           5000
#define kInitialBetAmount                      20

#define kSpaceBetweenCardAndEdge    4

#define BLACKJACK_DATA_FILE_NAME               "blackjack.dat"
#define BLACKJACK_DATA_FILE_VERSION            0

//   Bit values for play option flags
#define playopt_doubleDownAllowed              0x0001
//#define playopt_splitAllowed                   0x0002
#define playopt_insuranceBet                   0x0004

//   Bit values for device attribute flags
#define devattr_screenFitsCashInfo             0x0001

#define kNumberSuits                4
#define kNumberIdentities           13
#define kNumberCardsInADeck         (kNumberSuits * kNumberIdentities)
#define kNumberDecks                6
#define kTotalNumberCards           (kNumberCardsInADeck * kNumberDecks)
#define kLowerLimitLastCardToDeal   (kTotalNumberCards * 3/4)
#define kUpperLimitLastCardToDeal   (kTotalNumberCards * 9/10)
#define kMaxCardsInHand             22

/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/


// Save some typing later when malloc'ing or determining size of AECHAR buffers
#define MALLOC_AECHARS(nNumberChars)   MALLOC( nNumberChars * sizeof(AECHAR) )
#define SIZEOF_AECHARS(nNumberChars)   ( nNumberChars * sizeof(AECHAR) )


// Quick and dirty random number generator that's very fast and better than
// GET_RAND (which has no information about its periodicity and only
// generates 8 bit values).  It is a linear congruential generator with
// appropriate constants with a periodicity equal to the kRandomMod value.
// RandWithMax and RandRange provide a better method of getting a range from
// a given value than using mod. The pseudo random numbers generated by linear
// congruential generators have low order bits that are less random than the
// high order bits.   RandWithMax and RandRange uses the high order bits
// whereas mod uses the low order bits.
#define kRandomMod                              53125
#define kRandomMultiplier                       171
#define kRandomPrimeIncrement                   11213

#define RandMakeSeed(seed)         (seed % kRandomMod)

#define RandQuick(seedOrLastRand) (                                          \
      ( (seedOrLastRand * kRandomMultiplier + kRandomPrimeIncrement)) %      \
                                                                  kRandomMod )

#define RandWithMax(rand, max)  ( ((max+1) * rand) / kRandomMod )

#define RandRange(rand, low, high)     \
                     ( low + ((((high-low)+1) * rand) / kRandomMod) )


#define DealNextCard(pMe)        pMe->m_cardArr[pMe->m_nextCardToDeal++]
#define CardIDToKind(cardID)     ((CardKindEnum) (cardID % kNumberIdentities))

#define IsOptionOn(pMe,option)   ((pMe->m_playOptionFlags & option) != 0)
#define SetOptionOn(pMe,option)  ( pMe->m_playOptionFlags |= option )
#define SetOptionOff(pMe,option) ( pMe->m_playOptionFlags &= ~option )

#define IsDeviceAttributeOn(pMe,devattr)   \
          ((pMe->m_deviceAttributeFlags & devattr) != 0)
#define SetDeviceAttributeOn(pMe,devattr)   \
          ( pMe->m_deviceAttributeFlags |= devattr )

#ifdef FEATURE_LCD_TOUCH_ENABLE
#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif
#ifndef RELEASEIF
#define RELEASEIF(p) do { if (p) { IBASE_Release((IBase*)(p)); p = 0; } } while (0)
#endif

//#define SetDeviceAttributeOff(pMe,devattr) ( pMe->m_deviceAttributeFlags &= ~devattr )


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   card_Two = 0,
   card_Three,
   card_Four,
   card_Five,
   card_Six,
   card_Seven,
   card_Eight,
   card_Nine,
   card_Ten,
   card_Jack,
   card_Queen,
   card_King,
   card_Ace
} CardKindEnum;

#if 0
typedef enum {
   suit_Clubs = 0,
   suit_Spades,
   suit_Diamonds,
   suit_Hearts
} CardSuitEnum;
#endif


typedef uint8 CardIDType;


typedef enum {
   state_showingOpeningScreen = 0,
   state_showingContinueOrNewScreen,
   state_betweenHands,
   state_playerPlayingFirstTwoCards,
   state_playerPlaying,
   state_dealerPlaying,
   state_pausingToShowHandBeforeDeterminingWinner,
   state_pausingToShowCardShufflingMessage,
   state_choosingBetAmount,
   state_cashBelowBetMinimumScreen,
   state_betAmountGreaterThanCashScreen,
   state_showingInsuranceOffer,
   state_pausingToShowInsuranceResult
} PlayStateEnum;



typedef struct {
   AEEApplet   a;

   //
   //   Interface related data members
   //

   AEERect     m_rectScreen;    // Rectangle containing entire screen

   AEERect      m_rectInfoScreen;
   AEERect      m_rectEraseScreen;

   // Rectangle for currently active menu, if any
   AEERect     m_rectMenu;
   AEERect     m_rectCardHands; // Rectangle for card hands
   AEERect     m_rectCashInfo;  // Rectangle for cash info

   int         m_nFontHeight;   // Height (ascent + descent) of normal font
   IImage     *m_pCardImages;   // Images of a deck of cards (minus Jokers)
   uint16      m_cardImageHeight;       // Height of a single card image
   uint16      m_cardImageWidth;        // Width of a single card image
   // Width of preferred space between left of one card and left of next card
   uint16      m_cardPreferredSpacing;
   // Width of overlapped space between left of one card and left of next card
   uint16      m_cardOverlappedSpacing;
   IMenuCtl   *m_pDuringHandMenu;       // Menu for hitting or standing
   IMenuCtl   *m_pTempMenu;     // Any menu allocated only when it's needed
   IMenuCtl   *m_pCurrentMenu;  // Currently active menu
   IStatic    *m_pIStatic;      // Static for displaying information
   uint16      m_deviceAttributeFlags;  // Device attribute flags
   AECHAR     *m_szBuffer;      // Large buffer for string formatting

   //
   //   BlackJack persistent data
   //
   PlayStateEnum m_playState;   //   The current play state
   //   The last play state - used to go back to previous screen when allowed
   PlayStateEnum m_prevPlayState;
   CardIDType  m_cardArr[kTotalNumberCards];
   int         m_tableMinimum;
   int         m_tableMaximum;
   int         m_cash;
   int         m_initialBetAmount;
   int         m_handBetAmount;
   uint16      m_nextCardToDeal;
   uint16      m_lastCardToDeal;
   CardIDType  m_dealersHand[kMaxCardsInHand];
   CardIDType  m_playersHand[kMaxCardsInHand];
   uint8       m_dealersHandSize;
   uint8       m_playersHandSize;
   uint8       m_dealersHandValue;
   uint8       m_playersHandValue;
   uint16      m_playOptionFlags;       //   Current play options

   //   BlackJack runtime only data
   uint16      m_nInfoTitleResID;
   uint16      m_nMoneyResultResID;
} BlackJackApp;

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

// Local/Static Routines
static boolean BlackJack_InitAppData(BlackJackApp *pMe);
static void BlackJack_FreeAppData(BlackJackApp *pMe);

//      Event handling
static boolean BlackJack_HandleEvent(BlackJackApp *pMe,
                                     AEEEvent eCode,
                                     uint16 wParam,
                                     uint32 dwParam);
static void BlackJack_HandleSuspend(BlackJackApp *pMe);
static void BlackJack_RestoreState(BlackJackApp *pMe,
                                   PlayStateEnum state);
static void BlackJack_HandleStop(BlackJackApp *pMe);
static boolean BlackJack_HandleCommand(BlackJackApp *pMe,
                                       uint16 wParam);
static boolean BlackJack_HandleKey(BlackJackApp *pMe,
                                   uint16 wParam,
                                   uint32 dwParam);
static void BlackJack_SetState(BlackJackApp *pMe,
                               PlayStateEnum state);

//      Persistence
static void BlackJack_SaveGameData(BlackJackApp *pMe);
static boolean BlackJack_LoadGameData(BlackJackApp *pMe);

//      Opening screens
static void BlackJack_ShowOpeningScreen(BlackJackApp *pMe);
static void BlackJack_ShowContinueOrNewScreen(BlackJackApp *pMe);

//      Card handling
static void BlackJack_ShuffleCards(BlackJackApp *pMe);
static uint8 BlackJack_CalculateHandValue(CardIDType cardsArr[],
                                          int numberOfCards,
                                          boolean blackJackAllowed);

//      Hand drawing
static int  BlackJack_DrawHandValue(BlackJackApp *pMe,
                                    uint8 handValue,
                                    AECHAR * szHandValueFormat,
                                    int yPos,
                                    AEEFont font);
static void BlackJack_DrawHands(BlackJackApp *pMe,
                                boolean bDealerPlaying,
                                boolean bDrawPlayersHand);
static void BlackJack_DrawCashInfo(BlackJackApp *pMe);

static void BlackJack_ShowBetweenHandsInfo(BlackJackApp *pMe);
static void BlackJack_NewGame(BlackJackApp *pMe);
static void BlackJack_CheckStateAndDeal(BlackJackApp *pMe);

//      Bet handling functions
static void BlackJack_DisplayBetErrorScreen(BlackJackApp *pMe,
                                            PlayStateEnum state);
static boolean BlackJack_CheckBetAmount(BlackJackApp *pMe,
                                        boolean bCheckAll);
static boolean BlackJack_AddItemToChangeBetMenu(IMenuCtl * pMenu,
                                                boolean bLastAddResult,
                                                int nThreshold,
                                                int nCashRemaining,
                                                int nCurrentBet,
                                                uint16 nItemID,
                                                uint16 *nClosestItemID);
static void BlackJack_DisplayChangeBetMenu(BlackJackApp *pMe);
static void BlackJack_ChangeBetAmount(BlackJackApp *pMe,
                                      uint16 wParam);

//      Insurance handling functions
static void BlackJack_DisplayInsuranceOffer(BlackJackApp *pMe);
static boolean BlackJack_DisplayInsuranceStatic(BlackJackApp *pMe,
                                                uint16 nTitleResID,
                                                uint16 nMessageFormatResID,
                                                boolean bLeaveSpaceForMenu);
static void BlackJack_HandleInsuranceResponse(BlackJackApp *pMe,
                                              uint16 wParam);
static void BlackJack_ShowInsuranceResultMessageTimer(BlackJackApp *pMe);

static void BlackJack_ShuffleCardMessageDoneTimer(BlackJackApp *pMe);

//      Play handling functions
static void BlackJack_InitialDeal(BlackJackApp *pMe);
static void BlackJack_CheckForBlackJack(BlackJackApp *pMe);
static void BlackJack_Hit(BlackJackApp *pMe,
                          boolean doublingDown);
static void BlackJack_Stand(BlackJackApp *pMe);
static void BlackJack_DealerNextAction(BlackJackApp *pMe);
static void BlackJack_DealerPlayTimer(BlackJackApp *pMe);
static void BlackJack_DetermineWinnerTimer(BlackJackApp *pMe);

//      Utility routines that could be reused
static void Util_DrawCard(IImage * pCardImages,
                          CardIDType card,
                          int xPos,
                          int yPos,
                          uint16 cardImageHeight,
                          boolean bCardIsHidden);
static void Util_DrawCards(IImage * pCardImages,
                           CardIDType cardsArr[],
                           int numberOfCards,
                           int firstNonHiddenCard,
                           int xPos,
                           int yPos,
                           int widthAvailable,
                           uint16 cardImageHeight,
                           uint16 cardImageWidth,
                           uint16 cardImagePreferredSpacing,
                           uint16 cardImageOverlappedSpacing);
static void Util_AddString(AECHAR * pDest,
                           AECHAR * pText);
static void BlackJack_ReleaseControls(BlackJackApp *pMe);

#if defined(AEE_STATIC)

int BlackJack_Load(IShell   *ps,
                          void     *pHelpers,
                          IModule **pMod);

static int BJMod_CreateInstance(AEECLSID  clsId,
                               IShell   *pIShell,
                               IModule  *po,
                               void    **ppObj);
#endif //AEE_STATIC

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

#if defined(AEE_STATIC)
/*=============================================================================
FUNCTION: BlackJack_Load

DESCRIPTION:

PARAMETERS:
   *ps:
   *pHelpers:
   **pMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int BlackJack_Load(IShell   *ps,
                          void     *pHelpers,
                          IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST) BJMod_CreateInstance,
                           (PFNFREEMODDATA) NULL);
}

#endif //AEE_STATIC

/*=============================================================================
FUNCTION: BJMod_CreateInstance

DESCRIPTION:

PARAMETERS:
   clsId:
   *pIShell:
   *po:
   **ppObj:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if defined(AEE_STATIC)
static int BJMod_CreateInstance(AEECLSID  clsId,
                                IShell   *pIShell,
                                IModule  *po,
                                void    **ppObj)
#else
int AEEClsCreateInstance(AEECLSID  clsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
#endif
{
   int  createResult = ECLASSNOTSUPPORT;


   if (clsId == AEECLSID_BLACKJACK) {
      boolean createGood;
      // Allocate the applet class.
      createGood = AEEApplet_New(sizeof(BlackJackApp),
                                 clsId,
                                 pIShell,
                                 po,
                                 (IApplet **) ppObj,
                                 (AEEHANDLER) BlackJack_HandleEvent,
                                 (PFNFREEAPPDATA) BlackJack_FreeAppData);

      if (createGood) {
         // Init any app specific data
         BlackJackApp *pMe = (BlackJackApp *) *ppObj;

         // If init goes ok then return success
         if (BlackJack_InitAppData(pMe)) {
            createResult = AEE_SUCCESS;
         }
      } else {
         createResult = ENOMEMORY;
      }
   }

   return createResult;
}


/*=============================================================================
FUNCTION: BlackJack_InitAppData

DESCRIPTION: Initializes app specific data.

PARAMETERS:
   *pMe:  Pointer to the BlackJackApp structure.


RETURN VALUE:
   boolean:
              TRUE:   Initialization succeeded
              FALSE:  Initialization failed

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_InitAppData(BlackJackApp *pMe)
{
   IShell        *pShell = pMe->a.m_pIShell;
   AEEDeviceInfo  di;
   boolean        initGood = TRUE;

   // If initialization fails BlackJack_FreeAppData will be called. Make
   // sure that all pointers are initially NULL so that we don't attempt
   // to free random memory (BREW might be zero'ing our memory for us
   // in which case this isn't necessary, but it doesn't hurt to be
   // careful).
   pMe->m_pCardImages = NULL;
   pMe->m_pDuringHandMenu = NULL;
   pMe->m_pTempMenu = NULL;
   pMe->m_pCurrentMenu = NULL;
   pMe->m_pIStatic = NULL;
   pMe->m_szBuffer = NULL;

   // Get screen pixel count.
   ISHELL_GetDeviceInfo(pShell, &di);

   // Stash the screen rect
   SETAEERECT(&pMe->m_rectScreen, 0, 0, di.cxScreen, di.cyScreen);

   // Allocate a large buffer for string formatting
   pMe->m_szBuffer = MALLOC_AECHARS(512);
   if (pMe->m_szBuffer == NULL) {
      initGood = FALSE;
   }

   if (initGood) {
	pMe->m_pCardImages = ISHELL_LoadResImage(pShell,BLACKJACK_IMAGE_RES_FILE,IDI_CARDS);
      
      if (pMe->m_pCardImages != NULL) {
         AEEImageInfo imageInfo;

         IIMAGE_SetParm(pMe->m_pCardImages,
                        IPARM_NFRAMES,
                        kNumberCardImages,
                        0);
         IIMAGE_GetInfo(pMe->m_pCardImages, &imageInfo);
         pMe->m_cardImageHeight = (uint16) (imageInfo.cy * 2);
         pMe->m_cardImageWidth = imageInfo.cxFrame;
         pMe->m_cardPreferredSpacing =
                   (uint16) (pMe->m_cardImageWidth + kHorizSpaceBetweenCards);
         pMe->m_cardOverlappedSpacing =
                     (uint16) (pMe->m_cardImageWidth * kOverlapSpacingFactor);
      } else {
      	 DBGPRINTF("BlackJack_InitAppData....................................");
         initGood = FALSE;
      }
   }

   if (initGood) {
      // Get height of font.
      pMe->m_nFontHeight = IDISPLAY_GetFontMetrics(pMe->a.m_pIDisplay,
                                                   AEE_FONT_NORMAL,
                                                   NULL,
                                                   NULL);
      if (pMe->m_nFontHeight == EFAILED) {
         initGood = FALSE;
      }
   }

   // Set up the during hand menu
   if (initGood) {
      initGood = ( SUCCESS == ISHELL_CreateInstance(pShell,
                                                  AEECLSID_SOFTKEYCTL,
                                          (void **) &pMe->m_pDuringHandMenu) );
   }
   if (initGood) {
      initGood = IMENUCTL_AddItem(pMe->m_pDuringHandMenu,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  IDC_HIT,
                                  IDC_HIT,
                                  NULL,
                                  0);
   }
   if (initGood) {
      initGood = IMENUCTL_AddItem(pMe->m_pDuringHandMenu,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  IDC_STAND,
                                  IDC_STAND,
                                  NULL,
                                  0);
   }

   if (initGood) {
      int  deckNum,
           deckStart,
           i;
      int  cardAreaHeight;

      // Assign an ID for each card's suit/identity for all decks
      for (deckNum = 0; deckNum < kNumberDecks; deckNum++) {
         deckStart = deckNum * kNumberCardsInADeck;
         for (i = 0; i < kNumberCardsInADeck; i++) {
            pMe->m_cardArr[deckStart + i] = (CardIDType) i;
         }
      }

      // Shuffle the cards for the first time
      BlackJack_ShuffleCards(pMe);

      // Get the rect for the menu
      IMENUCTL_GetRect(pMe->m_pDuringHandMenu, &pMe->m_rectMenu);

      // The menu rect seems to be slightly too small (at least in the emulator
      // with a color device - the top line of the menu was not included). Even
      // if this is behavior is just a quirk and not necessary on
      // actual phones, wasting a single row of pixels is acceptable.
      pMe->m_rectMenu.y--;
      pMe->m_rectMenu.dy++;

      //      Set up the rect for the card area
      cardAreaHeight =
         (pMe->m_cardImageHeight * 2) + (kVerticalSpaceBetweenCards * 2);
      SETAEERECT(&pMe->m_rectCardHands, 
                 pMe->m_rectScreen.x+kSpaceBetweenCardAndEdge,
                 pMe->m_rectScreen.y+kSpaceBetweenCardAndEdge, 
                 pMe->m_rectScreen.dx-kSpaceBetweenCardAndEdge*2, 
                 cardAreaHeight);

      // Set up the rect for the info area
      SETAEERECT(&pMe->m_rectCashInfo,
                 pMe->m_rectScreen.x+kSpaceBetweenCardAndEdge,
                 cardAreaHeight+kSpaceBetweenCardAndEdge,
                 pMe->m_rectScreen.dx-kSpaceBetweenCardAndEdge*2,
                 (pMe->m_rectScreen.dy - pMe->m_rectMenu.dy) - cardAreaHeight-kSpaceBetweenCardAndEdge);

      SETAEERECT(&pMe->m_rectInfoScreen,
                 pMe->m_rectScreen.x+kSpaceBetweenCardAndEdge,
                 pMe->m_rectScreen.y+kSpaceBetweenCardAndEdge, 
                 pMe->m_rectScreen.dx-kSpaceBetweenCardAndEdge, 
                 pMe->m_rectScreen.dy-kSpaceBetweenCardAndEdge-pMe->m_rectMenu.dy);

      SETAEERECT(&pMe->m_rectEraseScreen,
                 pMe->m_rectScreen.x,
                 pMe->m_rectScreen.y, 
                 pMe->m_rectScreen.dx, 
                 pMe->m_rectScreen.dy-pMe->m_rectMenu.dy);

      // Start out in the opening screen state
      pMe->m_playState = state_showingOpeningScreen;
      pMe->m_prevPlayState = state_showingOpeningScreen;

      // All device attribute flags off initially
      pMe->m_deviceAttributeFlags = 0;

      // Default status info initially
      pMe->m_nInfoTitleResID = IDS_BETWEEN_HANDS_DEFAULT_TITLE;
      pMe->m_nMoneyResultResID = 0;

      // Is there enough room for the cash info? If not, then expand
      // m_rectCardHands vertically to include whatever space was assigned
      // to pMe->m_rectCashInfo so that we at least erase it.
      if (pMe->m_rectCashInfo.dy > (2 * pMe->m_nFontHeight)) {
         SetDeviceAttributeOn(pMe, devattr_screenFitsCashInfo);
      } else {
         pMe->m_rectCardHands.dy =
                    (short) (pMe->m_rectCardHands.dy + pMe->m_rectCashInfo.dy);
      }
   }

   return initGood;
}

/*=============================================================================
FUNCTION: BlackJack_FreeAppData

DESCRIPTION: Frees app specific data.

PARAMETERS:
   *pMe: Pointer to the BlackJackApp structure

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_FreeAppData(BlackJackApp *pMe)
{
   if (pMe->m_pCardImages) {
      IIMAGE_Release(pMe->m_pCardImages);
      pMe->m_pCardImages = NULL;
   }
   
   if(pMe->m_pCurrentMenu == pMe->m_pDuringHandMenu) {
      RELEASEIF(pMe->m_pCurrentMenu);
      pMe->m_pDuringHandMenu = NULL;
   }
   
   if (pMe->m_pCurrentMenu == pMe->m_pTempMenu) {
      RELEASEIF(pMe->m_pCurrentMenu);
      pMe->m_pTempMenu = NULL;
   }
   
   if (pMe->m_pCurrentMenu) {
      IMENUCTL_Release(pMe->m_pCurrentMenu);
      pMe->m_pCurrentMenu = NULL;
   }
   
   if (pMe->m_pDuringHandMenu) {
      IMENUCTL_Release(pMe->m_pDuringHandMenu);
      pMe->m_pDuringHandMenu = NULL;
   }
   
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }

   if (pMe->m_szBuffer) {
      FREE(pMe->m_szBuffer);
      pMe->m_szBuffer = NULL;
   }
}



/*=============================================================================
FUNCTION: BlackJack_HandleEvent

DESCRIPTION: Handles all events to BlackJack app.

PARAMETERS:
   *pMe:
   eCode:
   wParam:
   dwParam:

RETURN VALUE:
   boolean:
              TRUE:   Event processed
              FALSE:  Event not processed, defer to default handling

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_HandleEvent(BlackJackApp *pMe,
                                     AEEEvent      eCode,
                                     uint16        wParam,
                                     uint32        dwParam)
{
   boolean bEventHandled = TRUE;
   	#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
	if((eCode == EVT_PEN_UP)&&(pMe->m_playState ==state_choosingBetAmount))
	{
			AEEDeviceInfo devinfo;
			int nBarH ;
			AEERect rc;
			int16 wXPos = (int16)AEE_GET_X(dwParam);
			int16 wYPos = (int16)AEE_GET_Y(dwParam);

			nBarH = GetBottomBarHeight(pMe->a.m_pIDisplay);
    
			MEMSET(&devinfo, 0, sizeof(devinfo));
			ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &devinfo);
			SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

			if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
			{
				if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//×ó
				{
					int16 id = IMENUCTL_GetSel(pMe->m_pCurrentMenu);
					eCode = EVT_COMMAND;
					wParam = id;
				}
				else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//×ó
				{						
					 eCode = EVT_KEY;
					 wParam = AVK_CLR;
				}
			}
	}
#endif  

   switch (eCode) {
   case EVT_APP_SUSPEND:
      BlackJack_HandleSuspend(pMe);
      break;

   case EVT_APP_RESUME:
      BlackJack_RestoreState(pMe, pMe->m_playState);
      break;

   case EVT_APP_START:
	  BlackJack_SetState(pMe, state_showingOpeningScreen);
      break;

   case EVT_COPYRIGHT_END:
      BlackJack_SetState(pMe, state_showingOpeningScreen);
      break;

   case EVT_APP_STOP:
      BlackJack_HandleStop(pMe);
      break;

   case EVT_COMMAND:
      bEventHandled = BlackJack_HandleCommand(pMe, wParam);
      break;

   case EVT_KEY:
      bEventHandled = BlackJack_HandleKey(pMe, wParam, dwParam);
      break;

#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
      case EVT_PEN_UP:
	  	MSG_FATAL("EVT_PEN_UP...........pMe->m_playState=%d",pMe->m_playState,0,0);
        if ( (pMe->m_pIStatic != NULL) && ISTATIC_HandleEvent(pMe->m_pIStatic,
                                                              eCode,
                                                              wParam,
                                                              dwParam)) {
           return FALSE;
        }
 
        if (pMe->m_pCurrentMenu != NULL)
        {
            AEERect rc;
            int16 wXPos = (int16)AEE_GET_X(dwParam);
            int16 wYPos = (int16)AEE_GET_Y(dwParam);
            IMENUCTL_GetRect(pMe->m_pCurrentMenu,&rc); 
            if(PT_IN_RECT(wXPos,wYPos,rc))      
            {
                if(( pMe->m_playState == state_showingContinueOrNewScreen) ||
                   ( pMe->m_playState == state_betweenHands) ||
                   ( pMe->m_playState == state_cashBelowBetMinimumScreen) ||
                   ( pMe->m_playState == state_cashBelowBetMinimumScreen)
                   )
                {
                     if (ISHELL_GetActiveDialog(pMe->a.m_pIShell)) 
                     {
                        (void) ISHELL_EndDialog(pMe->a.m_pIShell);
                     }       
                }
                bEventHandled = IMENUCTL_HandleEvent(pMe->m_pCurrentMenu,
                                                eCode,
                                                wParam,
                                                dwParam);
            }
        }        
        break;
#endif            

   default:
      bEventHandled = FALSE;
      break;
   }

   return bEventHandled;
}


/*=============================================================================
FUNCTION: BlackJack_HandleSuspend

DESCRIPTION:
      Handles a suspend event by cleaning up anything that might be messy
      to continue when we are supposed to be dormant.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_HandleSuspend(BlackJackApp *pMe)
{
   // Cancel all timers. We will know what to do when we get the resume event
   // by our state - including any timer routine that should be called
   // immediately
// (void) ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);
   (void) ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)BlackJack_DealerPlayTimer, pMe);
   (void) ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)BlackJack_DetermineWinnerTimer, pMe);
   (void) ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)BlackJack_ShuffleCardMessageDoneTimer, pMe);
   (void) ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)BlackJack_ShowInsuranceResultMessageTimer, pMe);

   // Until we determine or get confimation that dialogs (IShellPrompts)
   // will be removed by a suspend event, we should make sure that they
   // are removed.
   if (ISHELL_GetActiveDialog(pMe->a.m_pIShell)) {
      (void) ISHELL_EndDialog(pMe->a.m_pIShell);
   }

   // Automatic scrolling of IStatic draws over keyguard. Remove IStatic and
   // reallocate on resume.
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }
}


/*=============================================================================
FUNCTION: BlackJack_RestoreState

DESCRIPTION:
      Do redrawing/redisplaying of appropriate interface elements given the
      specified state. Normally redraws are not needed, but when they are
      when we are resumed or when we have just loaded a game in progress
      from a file.

PARAMETERS:
   *pMe:
   state:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_RestoreState(BlackJackApp *pMe,
                                   PlayStateEnum state)
{
   PlayStateEnum prevState = pMe->m_playState;

   // First set the state to allow any everytime actions to occur
   // (or in the case of a restore from file first time actions as well).

  // if (state_choosingBetAmount != state)
   BlackJack_SetState(pMe, state);

   switch (state) {
   case state_playerPlayingFirstTwoCards:
   case state_playerPlaying:
      // The player was playing. Redraw the hands, cash info, and the menu.
      BlackJack_DrawHands(pMe, kDealerNotPlaying, kDrawPlayersHand);
      BlackJack_DrawCashInfo(pMe);
      if (prevState == state) {
         // Redraw the during hand menu (if the state is different then
         // we already drew the menu in BlackJack_SetState)
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectMenu);
         (void) IMENUCTL_Redraw(pMe->m_pDuringHandMenu);
      }
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      break;

   case state_dealerPlaying:
   case state_pausingToShowHandBeforeDeterminingWinner:
      // Redraw the cards so that the player can see what's going on - the
      // timer was set again in BlackJack_SetState
      BlackJack_DrawHands(pMe, kDealerPlaying, kDrawPlayersHand);
      BlackJack_DrawCashInfo(pMe);
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      break;

   case state_pausingToShowCardShufflingMessage:
      // We're done displaying the shuffle message now
      BlackJack_ShuffleCardMessageDoneTimer(pMe);
      break;

   case state_choosingBetAmount:
       BlackJack_DisplayChangeBetMenu(pMe);
 /*    if (prevState != state) {
         // Redraw the choose bet amount menu (if the state is different then
         // we already drew the menu in BlackJack_SetState)
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectMenu);
         (void) IMENUCTL_Redraw(pMe->m_pTempMenu);
             IDISPLAY_Update(pMe->a.m_pIDisplay);
      }*/
      break;

   case state_showingInsuranceOffer:
      // We were offering insurance. Redraw the hands so that the
      // dealer's hand is displayed.
      BlackJack_DrawHands(pMe, kDealerNotPlaying, kDontDrawPlayersHand);
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      break;

   case state_pausingToShowInsuranceResult:
      // We're done displaying the insurance result message now
      BlackJack_ShowInsuranceResultMessageTimer(pMe);
      break;
   default:
      break;
   }
}


/*=============================================================================
FUNCTION: BlackJack_HandleStop

DESCRIPTION:
      Handles a stop event by stopping all timers and saving the current play
      state.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_HandleStop(BlackJackApp *pMe)
{
   //      Cancel all our timers
   (void) ISHELL_CancelTimer(pMe->a.m_pIShell, NULL, pMe);

   //      Save game data
   BlackJack_SaveGameData(pMe);
}


/*=============================================================================
FUNCTION: BlackJack_HandleCommand

DESCRIPTION:
      Handles a command event (e.g. in response to a menu choice).

PARAMETERS:
   *pMe:
   wParam:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_HandleCommand(BlackJackApp *pMe,
                                       uint16        wParam)
{
   boolean bEventHandled = TRUE;
   switch (wParam) {
   case IDC_INSTRUCTIONS_OK:
      BlackJack_SetState(pMe, state_showingContinueOrNewScreen);
      break;

   case IDC_CONTINUE_GAME:
      if (BlackJack_LoadGameData(pMe))
         break;

      //lint -fallthrough

   case IDC_NEW_GAME:
   case IDC_NEW_GAME_SHORT_TITLE:
      BlackJack_NewGame(pMe);
      break;

   case IDC_DEAL:
       if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
       }
      BlackJack_CheckStateAndDeal(pMe);
      break;

   case IDC_CHANGE_BET:
       if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
       }
      BlackJack_SetState(pMe, state_choosingBetAmount);
      break;

   case IDC_BET_AMOUNT_10:
   case IDC_BET_AMOUNT_20:
   case IDC_BET_AMOUNT_50:
   case IDC_BET_AMOUNT_100:
   case IDC_BET_AMOUNT_150:
   case IDC_BET_AMOUNT_200:
      BlackJack_ChangeBetAmount(pMe, wParam);
      break;

   case IDC_HIT:
      BlackJack_Hit(pMe, kNormalHit);
      break;

   case IDC_STAND:
      BlackJack_Stand(pMe);
      break;

   case IDC_DOUBLE_DOWN:
      BlackJack_Hit(pMe, kDoublingDown);
      break;

   case IDC_WANT_INSURANCE:
   case IDC_NO_INSURANCE:
      BlackJack_HandleInsuranceResponse(pMe, wParam);
      break;

   default:
      bEventHandled = FALSE;
      break;
   }

   return bEventHandled;
}

/*=============================================================================
FUNCTION: BlackJack_HandleKey

DESCRIPTION: Handles a key event.

PARAMETERS:
   *pMe:
   wParam:
   dwParam:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_HandleKey(BlackJackApp *pMe,
                                   uint16        wParam,
                                   uint32        dwParam)
{
   boolean bEventHandled = FALSE;
#if defined( AEE_SIMULATOR)
    if( wParam == AVK_SOFT1)
#else
    if( wParam == AVK_INFO)
#endif
    {
            uint16 menuItemId =
                ( pMe->m_pCurrentMenu != NULL &&
                  IMENUCTL_IsActive( pMe->m_pCurrentMenu)
                ) ? IMENUCTL_GetSel( pMe->m_pCurrentMenu) : 0;

            if( menuItemId == 0)
            {
                return FALSE;
            }
            else
            {
                return ISHELL_PostEvent(pMe->a.m_pIShell,
                        AEECLSID_BLACKJACK,
                        EVT_COMMAND,
                        menuItemId,
                        0
                    );
            }
    }


   // Check to see if we are currently in a time delayed state. If so we
   // have an impatient user, cancel timer and call the function immediately.
   switch (pMe->m_playState) {
   case state_showingContinueOrNewScreen:
      if (((AVKType) wParam == AVK_1) || ((AVKType) wParam == AVK_2)) 
      {
         // Remove the "show continue or new" IShell_Prompt
         if (ISHELL_GetActiveDialog(pMe->a.m_pIShell)) 
         {
            (void) ISHELL_EndDialog(pMe->a.m_pIShell);
         }

         if ((AVKType) wParam == AVK_1) 
         {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_CONTINUE_GAME);
         } 
         else 
         {
            bEventHandled = BlackJack_HandleCommand(pMe,
                                                    IDC_NEW_GAME_SHORT_TITLE);
         }
      }
      if ((AVKType) wParam == AVK_CLR) 
      {
          BlackJack_ShowOpeningScreen(pMe);
          pMe->m_prevPlayState = state_showingOpeningScreen;
          pMe->m_playState = state_showingOpeningScreen;
          bEventHandled = TRUE;
      }
      break;

   case state_betweenHands:
      if (((AVKType) wParam == AVK_1) || ((AVKType) wParam == AVK_2)) 
        {
         // Remove the "between hands info" IShell_Prompt
         if (ISHELL_GetActiveDialog(pMe->a.m_pIShell)) 
         {
            (void) ISHELL_EndDialog(pMe->a.m_pIShell);
         }

         if ((AVKType) wParam == AVK_1) 
         {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_DEAL);
         } 
         else 
         {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_CHANGE_BET);
         }
      }
      break;

   case state_playerPlayingFirstTwoCards:
   case state_playerPlaying:
      switch ((AVKType) wParam) {
      case AVK_1:              //      Hit
         bEventHandled = BlackJack_HandleCommand(pMe, IDC_HIT);
         break;

      case AVK_2:              //      Stand
         bEventHandled = BlackJack_HandleCommand(pMe, IDC_STAND);
         break;

      case AVK_3:              //      Double down
         if ((pMe->m_playState == state_playerPlayingFirstTwoCards)
             && IsOptionOn(pMe, playopt_doubleDownAllowed))
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_DOUBLE_DOWN);
         break;

      case AVK_4:              //      Split
         break;
      default:
         break;
      }
      break;

   case state_dealerPlaying:
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                (PFNNOTIFY) BlackJack_DealerPlayTimer,
                                pMe);
      BlackJack_DealerPlayTimer(pMe);
      bEventHandled = TRUE;
      break;

   case state_pausingToShowHandBeforeDeterminingWinner:
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                                (PFNNOTIFY) BlackJack_DetermineWinnerTimer,
                                pMe);
      BlackJack_DetermineWinnerTimer(pMe);
      bEventHandled = TRUE;
      break;

   case state_pausingToShowCardShufflingMessage:
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                            (PFNNOTIFY) BlackJack_ShuffleCardMessageDoneTimer,
                                pMe);
      BlackJack_ShuffleCardMessageDoneTimer(pMe);
      bEventHandled = TRUE;
      break;

   case state_choosingBetAmount:
      if( wParam == AVK_CLR ||
          wParam == AVK_SOFT2
      )
      {
         // Go back to the "between hands info" IShell_Prompt without
         // changing the bet amount
		 MSG_FATAL("state_choosingBetAmount  AVK_SOFT2",0,0,0);
         // We're done with the bet amount menu, free the memory
         if (pMe->m_pTempMenu) {
            IMENUCTL_Release(pMe->m_pTempMenu);
            pMe->m_pTempMenu = NULL;
         }

         pMe->m_pCurrentMenu = NULL;
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
         // Go back to the last state (the status text or bet error message)
         BlackJack_SetState(pMe, pMe->m_prevPlayState);

         bEventHandled = TRUE;
      }
      break;

   case state_cashBelowBetMinimumScreen:
      if ((AVKType) wParam == AVK_1) {
         // Remove the "bet cash error" IShell_Prompt
         if (ISHELL_GetActiveDialog(pMe->a.m_pIShell)) {
            (void) ISHELL_EndDialog(pMe->a.m_pIShell);
         }

         bEventHandled = BlackJack_HandleCommand(pMe, IDC_NEW_GAME);
      }
      break;

   case state_betAmountGreaterThanCashScreen:
      if (((AVKType)wParam == AVK_1) || ((AVKType)wParam == AVK_2)) {
         // Remove the "bet cash error" IShell_Prompt
         if (ISHELL_GetActiveDialog(pMe->a.m_pIShell)) {
            (void) ISHELL_EndDialog(pMe->a.m_pIShell);
         }

         if ((AVKType) wParam == AVK_1) {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_CHANGE_BET);
         } else {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_NEW_GAME);
         }
      }
      break;

   case state_showingInsuranceOffer:
      if ( ((AVKType) wParam == AVK_1) ||
           ((AVKType) wParam == AVK_2) ||
           ((AVKType) wParam == AVK_CLR)) {

         if ((AVKType)  wParam == AVK_1) {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_WANT_INSURANCE);
         } else {
            bEventHandled = BlackJack_HandleCommand(pMe, IDC_NO_INSURANCE);
         }
      }
      break;

   case state_pausingToShowInsuranceResult:
      (void) ISHELL_CancelTimer(pMe->a.m_pIShell,
                         (PFNNOTIFY) BlackJack_ShowInsuranceResultMessageTimer,
                                pMe);
      BlackJack_ShowInsuranceResultMessageTimer(pMe);
      bEventHandled = TRUE;
      break;

   default:
      break;
   }
   if ( (pMe->m_pIStatic != NULL) && ISTATIC_HandleEvent(pMe->m_pIStatic,
                                                         EVT_KEY,
                                                         wParam,
                                                         dwParam)) 
   {
      return FALSE;
   }

   if (!bEventHandled && (pMe->m_pCurrentMenu != NULL)) 
    {
      bEventHandled = IMENUCTL_HandleEvent(pMe->m_pCurrentMenu,
                                           EVT_KEY,
                                           wParam,
                                           dwParam);
   }

   return bEventHandled;
}


/*=============================================================================
FUNCTION: BlackJack_SetState

DESCRIPTION:
      Sets the state of the app. The state is used to:
      * Keep track of time delayed events that we will execute immediately
        if the user presses a key
      * Consolidate and keep track of necessity of menu handling

PARAMETERS:
   *pMe:
   state: New state that the app is entering

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_SetState(BlackJackApp  *pMe,
                               PlayStateEnum  state)
{
   int result;

   if (state != pMe->m_playState) {
      // Perform state leaving actions
      switch (pMe->m_playState) {
      case state_playerPlayingFirstTwoCards:
         if (IsOptionOn(pMe, playopt_doubleDownAllowed)) {
            // Remove Double Down menu option
            (void) IMENUCTL_DeleteItem(pMe->m_pDuringHandMenu,
                                       IDC_DOUBLE_DOWN);

            // Redraw if the menu is going to still be around
            if (state == state_playerPlaying) {
               (void) IMENUCTL_Redraw(pMe->m_pDuringHandMenu);
            }
         }
         break;
      default:
         break;
      }

      //      Perform state entering actions
      switch (state) {
      case state_playerPlayingFirstTwoCards:
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectMenu);
         if (IsOptionOn(pMe, playopt_doubleDownAllowed)) {
            (void) IMENUCTL_AddItem(pMe->m_pDuringHandMenu,
                                    BLACKJACK_RES_FILE_LANGUAGE,
                                    IDC_DOUBLE_DOWN,
                                    IDC_DOUBLE_DOWN,
                                    NULL,
                                    NULL);
         }
         IMENUCTL_SetSel(pMe->m_pDuringHandMenu, IDC_HIT);
         IMENUCTL_SetActive(pMe->m_pDuringHandMenu, TRUE);
         pMe->m_pCurrentMenu = pMe->m_pDuringHandMenu;
         break;

      case state_playerPlaying:
         if (pMe->m_playState != state_playerPlayingFirstTwoCards) {
            // Set up the during hand menu if we didn't already do
            // so in the previous state
            IMENUCTL_SetSel(pMe->m_pDuringHandMenu, IDC_HIT);
            IMENUCTL_SetActive(pMe->m_pDuringHandMenu, TRUE);
            pMe->m_pCurrentMenu = pMe->m_pDuringHandMenu;
         }
         break;

      case state_dealerPlaying:
      case state_pausingToShowHandBeforeDeterminingWinner:
         IMENUCTL_SetActive(pMe->m_pDuringHandMenu, FALSE);
         pMe->m_pCurrentMenu = NULL;
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectMenu);
         break;

      case state_choosingBetAmount:
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
         BlackJack_DisplayChangeBetMenu(pMe);
         break;
      default:
         break;
      }
   }
   //      Perform every time actions
   switch (state) {
   case state_showingOpeningScreen:
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
      // Show the opening screen ISHELL_Prompt
      BlackJack_ShowOpeningScreen(pMe);
      break;

   case state_showingContinueOrNewScreen:
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
      // Show the continue game or new game ISHELL_Prompt
      BlackJack_ShowContinueOrNewScreen(pMe);
      break;

   case state_betweenHands:
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
      // Show the between hands status ISHELL_Prompt
      BlackJack_ShowBetweenHandsInfo(pMe);
      break;

   case state_dealerPlaying:
      // Pause so that the player can see the cards the dealer is hitting
      pMe->m_pCurrentMenu = NULL;
      result = ISHELL_SetTimer(pMe->a.m_pIShell,
                               kDealerPlayDelay,
                               (PFNNOTIFY) BlackJack_DealerPlayTimer,
                               pMe);

      // If the timer installation fails, call the timer function directly
      if (result != SUCCESS) {
         BlackJack_DealerPlayTimer(pMe);
      }
      break;

   case state_pausingToShowHandBeforeDeterminingWinner:
      // Pause so that the player can see the cards before
      // determining the winner
      pMe->m_pCurrentMenu = NULL;
      result = ISHELL_SetTimer(pMe->a.m_pIShell,
                               kDisplayResultsDelay,
                               (PFNNOTIFY) BlackJack_DetermineWinnerTimer,
                               pMe);
      // If the timer installation fails, call the timer function directly
      if (result != SUCCESS) {
         BlackJack_DetermineWinnerTimer(pMe);
      }
      break;

   case state_pausingToShowCardShufflingMessage:
      // Pause so that the player can read the card shuffling message
      pMe->m_pCurrentMenu = NULL;
      result = ISHELL_SetTimer(pMe->a.m_pIShell,
                               kShufflingMessageDelay,
                            (PFNNOTIFY) BlackJack_ShuffleCardMessageDoneTimer,
                               pMe);
      // If the timer installation fails, call the timer function directly
      if (result != SUCCESS) {
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
         BlackJack_CheckStateAndDeal(pMe);
      }
      break;

   case state_cashBelowBetMinimumScreen:
   case state_betAmountGreaterThanCashScreen:
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
      BlackJack_DisplayBetErrorScreen(pMe, state);
      break;

   case state_showingInsuranceOffer:
      BlackJack_DisplayInsuranceOffer(pMe);
      break;

   case state_pausingToShowInsuranceResult:
      //      Pause so that the player can read the insurance result message
      pMe->m_pCurrentMenu = NULL;
      result = ISHELL_SetTimer(pMe->a.m_pIShell,
                               kInsuranceResultMessageDelay,
                         (PFNNOTIFY) BlackJack_ShowInsuranceResultMessageTimer,
                               pMe);
      // If the timer installation fails, call the timer function directly
      if (result != SUCCESS) {
         BlackJack_ShowInsuranceResultMessageTimer(pMe);
      }
      break;
   default:
      break;
   }

   pMe->m_prevPlayState = pMe->m_playState;
   pMe->m_playState = state;
}


/*=============================================================================
FUNCTION: BlackJack_SaveGameData

DESCRIPTION:
      Saves the game data to blackjack.dat so that the player can continue
      the game the next time we're run

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_SaveGameData(BlackJackApp *pMe)
{
   IFileMgr   *pIFileMgr;
   int         result;

   result = ISHELL_CreateInstance(pMe->a.m_pIShell,
                                  AEECLSID_FILEMGR,
                                  (void **) &pIFileMgr);

   if (result == SUCCESS) {
      IFile      *pIFile;
      uint16      configFileVersion = BLACKJACK_DATA_FILE_VERSION;

      // Try to open existing file
      pIFile = IFILEMGR_OpenFile(pIFileMgr,
                                 BLACKJACK_DATA_FILE_NAME,
                                 _OFM_READWRITE);

      if (!pIFile) {
         // Failed - try to create
         pIFile = IFILEMGR_OpenFile(pIFileMgr,
                                    BLACKJACK_DATA_FILE_NAME,
                                    _OFM_CREATE);
      }

      if (pIFile) {
         // Reset the file
         (void) IFILE_Truncate(pIFile, 0);

         // Write out the file format version
         (void) IFILE_Write(pIFile,
                            &configFileVersion,
                            sizeof(configFileVersion));

         (void) IFILE_Write(pIFile,
                            &pMe->m_playState,
                            sizeof(pMe->m_playState));
         (void) IFILE_Write(pIFile,
                            &pMe->m_prevPlayState,
                            sizeof(pMe->m_prevPlayState));
         (void) IFILE_Write(pIFile,
                            pMe->m_cardArr,
                            sizeof(pMe->m_cardArr));
         (void) IFILE_Write(pIFile,
                            &pMe->m_tableMinimum,
                            sizeof(pMe->m_tableMinimum));
         (void) IFILE_Write(pIFile,
                            &pMe->m_tableMaximum,
                            sizeof(pMe->m_tableMaximum));
         (void) IFILE_Write(pIFile,
                            &pMe->m_cash,
                            sizeof(pMe->m_cash));
         (void) IFILE_Write(pIFile,
                            &pMe->m_initialBetAmount,
                            sizeof(pMe->m_initialBetAmount));
         (void) IFILE_Write(pIFile,
                            &pMe->m_handBetAmount,
                            sizeof(pMe->m_handBetAmount));
         (void) IFILE_Write(pIFile,
                            &pMe->m_nextCardToDeal,
                            sizeof(pMe->m_nextCardToDeal));
         (void) IFILE_Write(pIFile,
                            &pMe->m_lastCardToDeal,
                            sizeof(pMe->m_lastCardToDeal));
         (void) IFILE_Write(pIFile,
                            pMe->m_dealersHand,
                            sizeof(pMe->m_dealersHand));
         (void) IFILE_Write(pIFile,
                            pMe->m_playersHand,
                            sizeof(pMe->m_playersHand));
         (void) IFILE_Write(pIFile,
                            &pMe->m_dealersHandSize,
                            sizeof(pMe->m_dealersHandSize));
         (void) IFILE_Write(pIFile,
                            &pMe->m_playersHandSize,
                            sizeof(pMe->m_playersHandSize));
         (void) IFILE_Write(pIFile,
                            &pMe->m_playOptionFlags,
                            sizeof(pMe->m_playOptionFlags));

         IFILE_Release(pIFile);
      }

      IFILEMGR_Release(pIFileMgr);
   }
}


/*=============================================================================
FUNCTION: BlackJack_LoadGameData

DESCRIPTION:
      Loads game data from blackjack.dat and if the data looks good, continues
      play where the player last left off

PARAMETERS:
   *pMe:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_LoadGameData(BlackJackApp *pMe)
{
   IFileMgr   *pIFileMgr;
   int         result;

   boolean     dataLoadGood = FALSE;

   result =
      ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                            (void **) &pIFileMgr);

   if (result == SUCCESS) {
      IFile      *pIFile;

      // Try to open game data file
      pIFile =
         IFILEMGR_OpenFile(pIFileMgr, BLACKJACK_DATA_FILE_NAME, _OFM_READ);

      if (pIFile) {
         uint16        configFileVersion;
         PlayStateEnum playState,
                       prevPlayState;

         // Read in the file format version
         (void) IFILE_Read(pIFile,
                           &configFileVersion,
                           sizeof(configFileVersion));

         // Read in the file data supported by configFileVersion
         (void) IFILE_Read(pIFile, &playState, sizeof(playState));
         (void) IFILE_Read(pIFile, &prevPlayState, sizeof(prevPlayState));
         (void) IFILE_Read(pIFile, pMe->m_cardArr, sizeof(pMe->m_cardArr));
         (void) IFILE_Read(pIFile,
                           &pMe->m_tableMinimum,
                           sizeof(pMe->m_tableMinimum));
         (void) IFILE_Read(pIFile,
                           &pMe->m_tableMaximum,
                           sizeof(pMe->m_tableMaximum));
         (void) IFILE_Read(pIFile,
                           &pMe->m_cash,
                           sizeof(pMe->m_cash));
         (void) IFILE_Read(pIFile,
                           &pMe->m_initialBetAmount,
                           sizeof(pMe->m_initialBetAmount));
         (void) IFILE_Read(pIFile,
                           &pMe->m_handBetAmount,
                           sizeof(pMe->m_handBetAmount));
         (void) IFILE_Read(pIFile,
                           &pMe->m_nextCardToDeal,
                           sizeof(pMe->m_nextCardToDeal));
         (void) IFILE_Read(pIFile,
                           &pMe->m_lastCardToDeal,
                           sizeof(pMe->m_lastCardToDeal));
         (void) IFILE_Read(pIFile,
                           pMe->m_dealersHand,
                           sizeof(pMe->m_dealersHand));
         (void) IFILE_Read(pIFile,
                           pMe->m_playersHand,
                           sizeof(pMe->m_playersHand));
         (void) IFILE_Read(pIFile,
                           &pMe->m_dealersHandSize,
                           sizeof(pMe->m_dealersHandSize));
         (void) IFILE_Read(pIFile,
                           &pMe->m_playersHandSize,
                           sizeof(pMe->m_playersHandSize));
         (void) IFILE_Read(pIFile,
                           &pMe->m_playOptionFlags,
                           sizeof(pMe->m_playOptionFlags));

         // Sanity check the data
         if ((pMe->m_tableMinimum > 0) &&
             (pMe->m_tableMaximum > 0) &&
             (pMe->m_cash > 0) &&
             (pMe->m_initialBetAmount > 0) &&
             (pMe->m_nextCardToDeal < kTotalNumberCards) &&
             (pMe->m_lastCardToDeal > 0) &&
             (pMe->m_lastCardToDeal < kTotalNumberCards) &&
             (pMe->m_dealersHandSize <= kMaxCardsInHand) &&
             (pMe->m_playersHandSize <= kMaxCardsInHand)) {

            CardIDType  cardTallyArr[kNumberCardsInADeck];
            int         cardIndex;
            CardIDType  cardID;

            dataLoadGood = TRUE;

            // Sanity check the dealer's hand
            for (cardIndex = 0;
                 cardIndex < pMe->m_dealersHandSize;
                 cardIndex++) {

               if (pMe->m_dealersHand[cardIndex] >= kNumberCardsInADeck) {
                  dataLoadGood = FALSE;
                  break;
               }
            }

            if (dataLoadGood) {
               // Sanity check the player's hand
               for (cardIndex = 0; cardIndex < pMe->m_playersHandSize;
                    cardIndex++) {
                  if (pMe->m_playersHand[cardIndex] >= kNumberCardsInADeck) {
                     dataLoadGood = FALSE;
                     break;
                  }
               }
            }
            // Sanity check the decks to make sure that right numbers of
            // each card are still present.
            MEMSET(cardTallyArr, 0, sizeof(cardTallyArr));

            for (cardIndex = 0; cardIndex < kTotalNumberCards; cardIndex++) {
               cardID = pMe->m_cardArr[cardIndex];
               if (cardID >= kNumberCardsInADeck) {
                  dataLoadGood = FALSE;
                  break;
               }

               cardTallyArr[cardID]++;
            }

            if (dataLoadGood) {
               for (cardIndex = 0;
                    cardIndex < kNumberCardsInADeck;
                    cardIndex++) {
                  if (cardTallyArr[cardIndex] != kNumberDecks) {
                     dataLoadGood = FALSE;
                     break;
                  }
               }
            }
         }

         IFILE_Release(pIFile);

         if (dataLoadGood) {
            // Calculate the hand values
            pMe->m_playersHandValue =
               BlackJack_CalculateHandValue(pMe->m_playersHand,
                                            pMe->m_playersHandSize,
                                            kBlackJackAllowed);
            pMe->m_dealersHandValue =
               BlackJack_CalculateHandValue(pMe->m_dealersHand,
                                            pMe->m_dealersHandSize,
                                            kBlackJackAllowed);

            // First restore our current state
            BlackJack_RestoreState(pMe, playState);

            // Then set our previous state
            pMe->m_prevPlayState = prevPlayState;
         }
      }

      IFILEMGR_Release(pIFileMgr);
   }

   return dataLoadGood;
}

/*=============================================================================
FUNCTION: BlackJack_ShowOpeningScreen

DESCRIPTION: Shows the opening screen with instructions.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ShowOpeningScreen(BlackJackApp *pMe)
{
   IShell     *pShell = pMe->a.m_pIShell;
   uint16      wButtons[2];
   AEEPromptInfo bi;
   AECHAR     *szIntroText = pMe->m_szBuffer;
   AECHAR      sztitile[20];
   IFileMgr   *pIFileMgr;
   int         fileCheckResult = EFAILED;
//   AEERect     rc;

   BlackJack_ReleaseControls(pMe);
   pMe->m_pIStatic = NULL;

   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStatic)) {
      return ;
   }
   pMe->m_pCurrentMenu = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_SOFTKEYCTL,
                                            (void **)&pMe->m_pCurrentMenu)) {
      return ;
   }

   fileCheckResult =
      ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                            (void **) &pIFileMgr);

   if (fileCheckResult == SUCCESS) {
      // Check for existence of data file
      fileCheckResult = IFILEMGR_Test(pIFileMgr, BLACKJACK_DATA_FILE_NAME);

      IFILEMGR_Release(pIFileMgr);
   }

   MEMSET(&bi, 0, sizeof(AEEPromptInfo));

   if (fileCheckResult == SUCCESS) {
      // Our data file is present, so show the instructions ok button
      wButtons[0] = IDC_INSTRUCTIONS_OK;
      bi.wDefBtn = IDC_INSTRUCTIONS_OK;
   } else {
      // Our data file is missing, so show the new game button
      wButtons[0] = IDC_NEW_GAME;
      bi.wDefBtn = IDC_NEW_GAME;
   }
   /*
   wButtons[1] = 0;

   szIntroText[0] = 0;
   */
   (void) IMENUCTL_AddItem(pMe->m_pCurrentMenu,
                           BLACKJACK_RES_FILE_LANGUAGE,
                           wButtons[0],
                           (uint16) wButtons[0],
                           NULL,
                           NULL);

   // Load the string ourselves, because ISHELL_Prompt will only
   // load 128 characters.
   (void) ISHELL_LoadResString(pShell,
                               BLACKJACK_RES_FILE_LANGUAGE,
                               IDS_INSTRUCTIONS,
                               szIntroText,
                               SIZEOF_AECHARS(512));
/*
   ISTATIC_GetRect(pMe->m_pIStatic, &rc);
   IMENUCTL_GetRect(pMe->m_pCurrentMenu, &rc);*/
/*   SETAEERECT(&rc,pMe->m_rectInfoScreen.x,pMe->m_rectInfoScreen.y,
                   pMe->m_rectInfoScreen.dx+kSpaceBetweenCardAndEdge,pMe->m_rectInfoScreen.dy+1);
*/
   ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rectInfoScreen);
   ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL);
   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               BLACKJACK_RES_FILE_LANGUAGE,
                               IDS_TITLE,
                               sztitile,
                               sizeof(sztitile));
   (void) ISTATIC_SetText(pMe->m_pIStatic,
                          sztitile,
                          szIntroText,
                          AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);

   ISTATIC_SetActive( pMe->m_pIStatic, TRUE);
   IMENUCTL_SetActive(pMe->m_pCurrentMenu, TRUE);
   ISTATIC_Redraw(pMe->m_pIStatic);
//   IMENUCTL_Redraw(pMe->m_pCurrentMenu);
//   DBGPRINTF( ";BlackJack_ShowOpeningScreen, 7");
   /*
   bi.pszRes = BLACKJACK_RES_FILE_LANGUAGE;
   bi.wTitleID = IDS_TITLE;
   bi.pText = szIntroText;
   bi.dwProps = ST_CENTERTITLE;
   bi.pBtnIDs = (const uint16 *) wButtons;
   (void) ISHELL_Prompt(pShell, &bi);
   */
}
/*=============================================================================
FUNCTION: BlackJack_ShowContinueOrNewScreen

DESCRIPTION: Shows the continue last saved game or start new game prompt.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ShowContinueOrNewScreen(BlackJackApp *pMe)
{
   AECHAR     szBuff[128];
   AECHAR     sztitile[20];

   BlackJack_ReleaseControls(pMe);
   pMe->m_pIStatic = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStatic)) {
      return ;
   }
   pMe->m_pCurrentMenu = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_SOFTKEYCTL,
                                            (void **)&pMe->m_pCurrentMenu)) {
      return ;
   }
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectEraseScreen);
   (void) IMENUCTL_AddItem(pMe->m_pCurrentMenu,
                           BLACKJACK_RES_FILE_LANGUAGE,
                           IDC_CONTINUE_GAME,
                           (uint16) IDC_CONTINUE_GAME,
                           NULL,
                           NULL);
   (void) IMENUCTL_AddItem(pMe->m_pCurrentMenu,
                           BLACKJACK_RES_FILE_LANGUAGE,
                           IDC_NEW_GAME_SHORT_TITLE,
                           (uint16) IDC_NEW_GAME_SHORT_TITLE,
                           NULL,
                           NULL);

   ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL);
   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               BLACKJACK_RES_FILE_LANGUAGE,
                               IDS_CONTINUE_OR_NEW_TEXT,
                               szBuff,
                               sizeof(szBuff));
   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               BLACKJACK_RES_FILE_LANGUAGE,
                               IDS_TITLE,
                               sztitile,
                               sizeof(sztitile));

   ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rectInfoScreen);
   
   (void) ISTATIC_SetText(pMe->m_pIStatic,
                          sztitile,
                          szBuff,
                          AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);

   IMENUCTL_SetActive(pMe->m_pCurrentMenu, TRUE);
   (void) ISTATIC_Redraw(pMe->m_pIStatic);
   (void) IMENUCTL_Redraw(pMe->m_pCurrentMenu);

}


/*=============================================================================
FUNCTION: BlackJack_ShuffleCards

DESCRIPTION:
      Shuffle the cards in a quick and reasonably random fashion.
      Call before beginning a new game and between hands whenever
      m_nextCardToDeal >= m_lastCardToDeal. m_lastCardToDeal is chosen such
      that we are guaranteed to have enough cards to finish out the current
      hand.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ShuffleCards(BlackJackApp *pMe)
{
   uint32      i;
   uint32      randomNumber;
   uint32      randomCardIndex;
   int         randomCardID;
   CardIDType *cardArr = pMe->m_cardArr;

   // Get a seed for the random number generator
   randomNumber = RandMakeSeed(GET_SECONDS());

   for (i = kTotalNumberCards - 1; i > 0; i--) {
      // Get the next pseudo random number
      randomNumber = RandQuick(randomNumber);

      // Make it be between 0 and i, inclusive
      randomCardIndex = RandWithMax(randomNumber, i);

      // It's possible for the card to end up in its original position,
      // in which case we don't need to do anything. (This possibility is
      // desirable given an even distribution of the possible shuffling
      // results).
      if (randomCardIndex != i) {
         // Swap the i'th card with the randomly chosen card
         randomCardID = cardArr[randomCardIndex];
         cardArr[randomCardIndex] = cardArr[i];
         cardArr[i] = (CardIDType) randomCardID;
      }
   }

   //      Avoid card counters - stop before we run out of cards
   randomNumber = RandQuick(randomNumber);
   pMe->m_lastCardToDeal = (uint16) RandRange(randomNumber,
                                              kLowerLimitLastCardToDeal,
                                              kUpperLimitLastCardToDeal);

   // We just shuffled, so start with the top of the deck
   pMe->m_nextCardToDeal = 0;
}


/*=============================================================================
FUNCTION: BlackJack_CalculateHandValue

DESCRIPTION: Calculate a hand value detecting BlackJack if it is allowed.

PARAMETERS:
   cardsArr[]: Array of cards for which to calculate value
   numberOfCards: Number of cards in cardsArr
   blackJackAllowed: Whether or not BlackJack is allowed (it's not
                     if the player has split his or her hand).

RETURN VALUE:
   uint8:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint8 BlackJack_CalculateHandValue(CardIDType cardsArr[],
                                          int        numberOfCards,
                                          boolean    blackJackAllowed)
{
   int          handValue = 0;
   CardKindEnum cardKind;

   // If BlackJack is allowed then check for it
   if (blackJackAllowed && (numberOfCards == 2)) {
      CardKindEnum otherCardKind;

      cardKind = CardIDToKind(cardsArr[0]);

      // If one card is an Ace then check the value of the other card
      if (cardKind == card_Ace) {
         otherCardKind = CardIDToKind(cardsArr[1]);
      } else {
         otherCardKind = cardKind;
         cardKind = CardIDToKind(cardsArr[1]);
      }

      if (cardKind == card_Ace) {
         if ( (otherCardKind == card_Ten) ||
              (otherCardKind == card_Jack) ||
              (otherCardKind == card_Queen) ||
              (otherCardKind == card_King)) {
            handValue = kBlackJackValue;
         }
      }
   }
   // If we don't have BlackJack, then add up the hand value
   if (handValue != kBlackJackValue) {
      uint8       i;
      uint8       numAces = 0;

      for (i = 0; i < numberOfCards; i++) {
         cardKind = CardIDToKind(cardsArr[i]);

         switch (cardKind) {
         case card_Two:
         case card_Three:
         case card_Four:
         case card_Five:
         case card_Six:
         case card_Seven:
         case card_Eight:
         case card_Nine:
            // The cardKind is 0 based, but the values start the count at 2.
            handValue += (int) cardKind + 2;
            break;

         case card_Ten:
         case card_Jack:
         case card_Queen:
         case card_King:
            handValue += 10;
            break;

         case card_Ace:
            //      Score aces as soft (1 point) by default
            numAces++;
            handValue++;
            break;
         }
      }

      // If we have one or more aces, should we score one of them as hard?
      // 11 is the value of a hard ace, so just add 10 to what we
      // already valued it at.
      if ((numAces > 0) && ((handValue + 10) <= kHandValueMax))
         handValue += 10;
   }
   return (uint8) handValue;
}


/*=============================================================================
FUNCTION: BlackJack_DrawHandValue

DESCRIPTION:
   Draws the hand value right aligned in the vertical position indicated

PARAMETERS:
   *pMe: Pointer to the BlackJackApp structure
   handValue: The value to draw
   *szHandValueFormat: The format for the value (currently just "%d")
   yPos: The vertical location to draw the value
   font:

RETURN VALUE:
   int: The width of the hand value string

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int BlackJack_DrawHandValue(BlackJackApp *pMe,
                                   uint8         handValue,
                                   AECHAR       *szHandValueFormat,
                                   int           yPos,
                                   AEEFont       font)
{
   int         handValueWidth = 0;
   AECHAR      szHandValue[16];
   AEERect  rectHandValueSreen;

   SETAEERECT(&rectHandValueSreen,
                        0,
                        0,
                        pMe->m_rectScreen.dx-kSpaceBetweenCardAndEdge+1,
                        pMe->m_rectScreen.dy);

   if (handValue != kBlackJackValue) {
      //  The value is not BlackJack, so sprintf the value
      WSPRINTF(szHandValue, sizeof(szHandValue), szHandValueFormat, handValue);

      // Calculate the width of the hand value string
      handValueWidth = IDISPLAY_MeasureText(pMe->a.m_pIDisplay,
                                            font,
                                            szHandValue);
   } else {
      // The value is BlackJack, so load the BlackJack string
      // zero the szHandValue so that we have a harmless value if load fails
      szHandValue[0] = 0;

      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  IDS_BLACKJACK_HAND,
                                  szHandValue,
                                  sizeof(szHandValue));

      // Don't calculate the width of the BlackJack string because we are
      // assuming that two cards can fit without overlapping the BlackJack
      // string
   }

   // Draw the hand value string
   (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                            font,
                            szHandValue,
                            -1,
                            0,
                            yPos,
                            &rectHandValueSreen,
                            IDF_ALIGN_RIGHT);

   return handValueWidth;
}


/*=============================================================================
FUNCTION: BlackJack_DrawHands

DESCRIPTION:
      Draws two rows of cards, one for the dealer and one for the player.
      To the right of the cards the current score of the player and dealer
      (if appropriate) is shown.

PARAMETERS:
   *pMe:
   bDealerPlaying: Indicates whether or not the dealer is currently playing.
                   The dealers hand and hand value are not shown otherwise.
   bDrawPlayersHand: Indicates whether of not to draw the player's hand

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DrawHands(BlackJackApp *pMe,
                                boolean       bDealerPlaying,
                                boolean       bDrawPlayersHand)
{
   int    playersYPos = pMe->m_cardImageHeight + kVerticalSpaceBetweenCards+kSpaceBetweenCardAndEdge;

   int    dealersFirstShownCard = bDealerPlaying ? 0 : 1;
   AECHAR szHandValueFormat[8];
   int    handValueWidth = 0;
   int    cardDrawingAvailableWidth;

   if (bDrawPlayersHand) {
      IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectCardHands);
   } else {
      // Erase just the card area for the dealer's hand
      int16 cardAreaHeight = pMe->m_rectCardHands.dy;

      pMe->m_rectCardHands.dy =
                 (int16) (pMe->m_cardImageHeight + kVerticalSpaceBetweenCards);
      IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectCardHands);
      pMe->m_rectCardHands.dy = cardAreaHeight;
   }

   // Load the format string for a hand value
   // (just a simple "%d" at the moment)
   if (ISHELL_LoadResString(pMe->a.m_pIShell,
                            BLACKJACK_RES_FILE_LANGUAGE,
                            IDS_HAND_VALUE_FORMAT,
                            szHandValueFormat,
                            sizeof(szHandValueFormat))) {
      int yPos;

      if (bDrawPlayersHand) {
         AEEFont     playerFont;

         // Center the hand value string vertically in line
         // with the player's cards
         yPos = pMe->m_cardImageHeight + kVerticalSpaceBetweenCards + kSpaceBetweenCardAndEdge +
                pMe->m_cardImageHeight / 2 - pMe->m_nFontHeight / 2;

         // Draw busts and BlackJacks in bold - both values are > kHandValueMax
         playerFont = (pMe->m_playersHandValue > kHandValueMax) ?
                                               AEE_FONT_BOLD : AEE_FONT_NORMAL;
         handValueWidth = BlackJack_DrawHandValue(pMe,
                                                  pMe->m_playersHandValue,
                                                  szHandValueFormat,
                                                  yPos,
                                                  playerFont);
      }

      if (bDealerPlaying) {
         // The dealer is playing, draw the dealer's hand value
         int  dealersHandValueWidth;

         // Center the hand value string vertically in line
         // with the dealer's cards
         yPos = pMe->m_cardImageHeight / 2 - pMe->m_nFontHeight / 2 + kSpaceBetweenCardAndEdge;

         dealersHandValueWidth = BlackJack_DrawHandValue(pMe,
                                                       pMe->m_dealersHandValue,
                                                       szHandValueFormat,
                                                       yPos,
                                                       AEE_FONT_NORMAL);

         //      Use the wider value
         if (dealersHandValueWidth > handValueWidth)
            handValueWidth = dealersHandValueWidth;
      }
   }

   cardDrawingAvailableWidth = (pMe->m_rectCardHands.dx - handValueWidth) - 2;

   Util_DrawCards(pMe->m_pCardImages,
                  pMe->m_dealersHand,
                  pMe->m_dealersHandSize,
                  dealersFirstShownCard,
                  kSpaceBetweenCardAndEdge,
                  kSpaceBetweenCardAndEdge,
                  cardDrawingAvailableWidth,
                  pMe->m_cardImageHeight,
                  pMe->m_cardImageWidth,
                  pMe->m_cardPreferredSpacing,
                  pMe->m_cardOverlappedSpacing);

   if (bDrawPlayersHand) {
      Util_DrawCards(pMe->m_pCardImages,
                     pMe->m_playersHand,
                     pMe->m_playersHandSize,
                     kShowAllCards,
                     kSpaceBetweenCardAndEdge,
                     playersYPos,
                     cardDrawingAvailableWidth,
                     pMe->m_cardImageHeight,
                     pMe->m_cardImageWidth,
                     pMe->m_cardPreferredSpacing,
                     pMe->m_cardOverlappedSpacing);
   }
}


/*=============================================================================
FUNCTION: BlackJack_DrawCashInfo

DESCRIPTION:

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DrawCashInfo(BlackJackApp *pMe)
{
   if (IsDeviceAttributeOn(pMe, devattr_screenFitsCashInfo)) {
      int         nNumBytesLoaded;
      AECHAR     *szFormattedBuf;
      AECHAR     *szRawBuf;

      // We have plenty of room for our large buffer because the
      // formatted cash status and bet amounts never end up being very long
      szFormattedBuf = pMe->m_szBuffer;
      szFormattedBuf[0] = 0;

      szRawBuf = &pMe->m_szBuffer[100];
      szRawBuf[0] = 0;

      IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectCashInfo);

      // Load the cash status string and fill it in
      nNumBytesLoaded = ISHELL_LoadResString(pMe->a.m_pIShell,
                                             BLACKJACK_RES_FILE_LANGUAGE,
                                             IDS_CASH_STATUS_FORMAT,
                                             szRawBuf,
                                             SIZEOF_AECHARS(100));
      if (nNumBytesLoaded) {
         WSPRINTF(szFormattedBuf, SIZEOF_AECHARS(100), szRawBuf, pMe->m_cash);

         // Draw the current cash amount and bet amount if there's
         // enough vertical screen space
         (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                  AEE_FONT_NORMAL,
                                  szFormattedBuf,
                                  -1 /*nChars */ ,
                                  kSpaceBetweenCardAndEdge,
                                  pMe->m_rectCashInfo.y,
                                  NULL /*cliprect */,
                                  0 /*dwFlags */ );
      }
      // Load the bet amount string and fill it in
      nNumBytesLoaded = ISHELL_LoadResString(pMe->a.m_pIShell,
                                             BLACKJACK_RES_FILE_LANGUAGE,
                                             IDS_BET_STATUS_FORMAT,
                                             szRawBuf,
                                             SIZEOF_AECHARS(100));
      if (nNumBytesLoaded) {
         WSPRINTF(szFormattedBuf, SIZEOF_AECHARS(100), szRawBuf,
                  pMe->m_handBetAmount);

         // Draw the current cash amount and bet amount if there's
         // enough vertical screen space
         (void) IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                                  AEE_FONT_NORMAL,
                                  szFormattedBuf,
                                  -1 /*nChars */ ,
                                  kSpaceBetweenCardAndEdge,
                                  pMe->m_rectCashInfo.y + pMe->m_nFontHeight,
                                  NULL /*cliprect */,
                                  0 /*dwFlags */ );
      }
   }
}


/*=============================================================================
FUNCTION: BlackJack_ShowBetweenHandsInfo

DESCRIPTION:
      Shows an ISHELL_Prompt with the current status (cash and bet amount) and
      the results of the last hand if appropriate. Offers the user the choice
      to deal the next hand or change the bet amount.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ShowBetweenHandsInfo(BlackJackApp *pMe)
{
   AECHAR        *szBetweenHandsInfo;
   AECHAR        *szLargeBuf;
   AECHAR         sztitile[20];
   IShell        *pShell = pMe->a.m_pIShell;
   AECHAR         szSmallBuf[16];
   int            nNumBytesLoaded;
   // 128 characters should be plenty for the largest string we could build,
   // but use 160 characters to leave a little extra space to be sure
   szBetweenHandsInfo = pMe->m_szBuffer;
   szBetweenHandsInfo[0] = 0;

   // Currently largest string is IDS_PUSH_NO_WINNINGS with 80 characters,
   // so we have enough room after szBetweenHandsInfo
   szLargeBuf = &pMe->m_szBuffer[160];
   szLargeBuf[0] = 0;

   szBetweenHandsInfo[0] = 0;

   // If we've been provided with a winnings string ID, load it
   if (pMe->m_nMoneyResultResID != 0) {
      nNumBytesLoaded = ISHELL_LoadResString(pShell,
                                             BLACKJACK_RES_FILE_LANGUAGE,
                                             pMe->m_nMoneyResultResID,
                                             szLargeBuf,
                                             SIZEOF_AECHARS(96));
      if (nNumBytesLoaded) {
         // If the hand was not a push, fill in the amount of money
         if (pMe->m_handBetAmount != 0) {
            WSPRINTF(szBetweenHandsInfo,
                     SIZEOF_AECHARS(160),
                     szLargeBuf,
                     pMe->m_handBetAmount);
         } else {
            WSTRCPY(szBetweenHandsInfo, szLargeBuf);
         }
      }
   }
   //      Load the cash status string and fill it in
   nNumBytesLoaded = ISHELL_LoadResString(pShell,
                                          BLACKJACK_RES_FILE_LANGUAGE,
                                          IDS_CASH_STATUS_FORMAT,
                                          szSmallBuf,
                                          sizeof(szSmallBuf));
   if (nNumBytesLoaded) {
      WSPRINTF(szLargeBuf, SIZEOF_AECHARS(96), szSmallBuf, pMe->m_cash);

      if (pMe->m_nMoneyResultResID != 0) {
         Util_AddString(szBetweenHandsInfo, szLargeBuf);
      } else {
         WSTRCPY(szBetweenHandsInfo, szLargeBuf);
      }
   }
   // Load the bet amount string and fill it in
   nNumBytesLoaded = ISHELL_LoadResString(pShell,
                                          BLACKJACK_RES_FILE_LANGUAGE,
                                          IDS_BET_STATUS_FORMAT,
                                          szSmallBuf,
                                          sizeof(szSmallBuf));
   if (nNumBytesLoaded) {
      WSPRINTF(szLargeBuf, SIZEOF_AECHARS(96), szSmallBuf,
               pMe->m_initialBetAmount);

      Util_AddString(szBetweenHandsInfo, szLargeBuf);
   }

   BlackJack_ReleaseControls(pMe);
   pMe->m_pIStatic = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStatic)) {
      return ;
   }
   pMe->m_pCurrentMenu = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pShell,
                                            AEECLSID_SOFTKEYCTL,
                                            (void **)&pMe->m_pCurrentMenu)) {
      return ;
   }
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectEraseScreen);
   (void) IMENUCTL_AddItem(pMe->m_pCurrentMenu,
                           BLACKJACK_RES_FILE_LANGUAGE,
                           IDC_DEAL,
                           (uint16) IDC_DEAL,
                           NULL,
                           NULL);
   (void) IMENUCTL_AddItem(pMe->m_pCurrentMenu,
                           BLACKJACK_RES_FILE_LANGUAGE,
                           IDC_CHANGE_BET,
                           (uint16) IDC_CHANGE_BET,
                           NULL,
                           NULL);


   ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rectInfoScreen);

   ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL);
   //ÏÔÊ¾¾²Ì¬ÎÄ±¾
   (void) ISHELL_LoadResString(pShell,
                               BLACKJACK_RES_FILE_LANGUAGE,
                               pMe->m_nInfoTitleResID,
                               sztitile,
                               sizeof(sztitile));
   
   (void) ISTATIC_SetText(pMe->m_pIStatic,
                          sztitile,
                          szBetweenHandsInfo,
                          AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);
   
   IMENUCTL_SetSel(pMe->m_pCurrentMenu, IDC_DEAL);
   IMENUCTL_SetActive(pMe->m_pCurrentMenu, TRUE);      
  
   (void) ISTATIC_Redraw(pMe->m_pIStatic);
   (void) IMENUCTL_Redraw(pMe->m_pCurrentMenu);
}


/*=============================================================================
FUNCTION: BlackJack_NewGame

DESCRIPTION: Handle the IDC_NEW_GAME command by starting a new game

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_NewGame(BlackJackApp *pMe)
{
   //      To do: Make cash and table parameters configurable
   pMe->m_tableMinimum = kTableMinimum;
   pMe->m_tableMaximum = kTableMaximum;
   pMe->m_cash = kInitialCash;
   pMe->m_initialBetAmount = kInitialBetAmount;
   pMe->m_handBetAmount = 0;
   pMe->m_nInfoTitleResID = IDS_BETWEEN_HANDS_DEFAULT_TITLE;
   pMe->m_nMoneyResultResID = 0;

   BlackJack_SetState(pMe, state_betweenHands);
}


/*=============================================================================
FUNCTION: BlackJack_CheckStateAndDeal

DESCRIPTION:
      Check to see if we need to shuffle before dealing. If so shuffle and
      display a message to the user about this with a timer so that we
      automatically deal after the user has been given a chance to see the
      message.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_CheckStateAndDeal(BlackJackApp *pMe)
{
   if (BlackJack_CheckBetAmount(pMe, kBetCheckAll)) {
      //      Is it time to shuffle?
      if (pMe->m_nextCardToDeal >= pMe->m_lastCardToDeal) {
         int         result = SUCCESS;

         BlackJack_ShuffleCards(pMe);

         // Display the shuffling message in our static.
         // We'll display the message for a few seconds and then remove
         // it automatically.
         if (pMe->m_pIStatic == NULL)
            result =
               ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC,
                                     (void **) &pMe->m_pIStatic);

         if (result == SUCCESS) {
            AECHAR      szShufflingTitle[16];
            AECHAR     *szShufflingTextFormat;
            AECHAR     *szShufflingText;
            AEERect     rc;
            SETAEERECT(&rc, 
                        pMe->m_rectScreen.x+kSpaceBetweenCardAndEdge,
                        pMe->m_rectScreen.y+kSpaceBetweenCardAndEdge, 
                        pMe->m_rectScreen.dx-kSpaceBetweenCardAndEdge, 
                        pMe->m_rectScreen.dy-kSpaceBetweenCardAndEdge);
            // Use 128 characters for szShufflingTextFormat
            szShufflingTextFormat = pMe->m_szBuffer;
            szShufflingTextFormat[0] = 0;

            // Use 128 characters for szShufflingText after
            // szShufflingTextFormat
            szShufflingText = &pMe->m_szBuffer[128];
            szShufflingText[0] = 0;

            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        BLACKJACK_RES_FILE_LANGUAGE,
                                        IDS_SHUFFLING_TITLE,
                                        szShufflingTitle,
                                        sizeof(szShufflingTitle));

            (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                        BLACKJACK_RES_FILE_LANGUAGE,
                                        IDS_SHUFFLING_MSG,
                                        szShufflingTextFormat,
                                        SIZEOF_AECHARS(128));

            WSPRINTF(szShufflingText, SIZEOF_AECHARS(128),
                     szShufflingTextFormat, kNumberDecks);
            IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectScreen);
            ISTATIC_SetRect(pMe->m_pIStatic, &rc);
            ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL);
            (void) ISTATIC_SetText(pMe->m_pIStatic,
                                   szShufflingTitle,
                                   szShufflingText,
                                   AEE_FONT_BOLD,
                                   AEE_FONT_NORMAL);

            ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rectInfoScreen);
            (void) ISTATIC_Redraw(pMe->m_pIStatic);
            IDISPLAY_Update(pMe->a.m_pIDisplay);

            BlackJack_SetState(pMe, state_pausingToShowCardShufflingMessage);
         } else {
            //      Static creation failed, just deal
            BlackJack_InitialDeal(pMe);
         }
      } else {
         BlackJack_InitialDeal(pMe);
      }
   }
}


/*=============================================================================
FUNCTION: BlackJack_DisplayBetErrorScreen

DESCRIPTION:
      Display a prompt with an error message regarding the bet amount
      and one or two options (if appropriate).

PARAMETERS:
   *pMe:
   state: State that indicates which bet error message to display.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DisplayBetErrorScreen(BlackJackApp *pMe,
                                            PlayStateEnum state)
{
   uint16      wButtons[3];
   uint16      titleID = 0;
   AECHAR     *szTextFormat = NULL;
   AECHAR     *szText = NULL;
   AECHAR      szBuff[20];
   uint16      i;

   // We need 96 characters for szTextFormat
   szTextFormat = pMe->m_szBuffer;
   szTextFormat[0] = 0;

   // We need 128 characters for szText - use m_szBuffer after szTextFormat
   szText = &pMe->m_szBuffer[96];
   szText[0] = 0;

   switch (state) {
   case state_cashBelowBetMinimumScreen:
      wButtons[0] = IDC_NEW_GAME;
      wButtons[1] = 0;

      titleID = IDS_BET_INSUFFICIENT_CASH_TITLE;

      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  IDS_BET_INSUFFICIENT_CASH_MSG,
                                  szTextFormat,
                                  SIZEOF_AECHARS(96));

      WSPRINTF(szText,
               SIZEOF_AECHARS(128),
               szTextFormat,
               pMe->m_tableMinimum,
               pMe->m_cash);
      break;

   case state_betAmountGreaterThanCashScreen:
      wButtons[0] = IDC_CHANGE_BET;
      wButtons[1] = IDC_NEW_GAME;
      wButtons[2] = 0;

      titleID = IDS_BET_LOW_CASH_TITLE;

      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  IDS_BET_LOW_CASH_MSG,
                                  szTextFormat,
                                  SIZEOF_AECHARS(96));

      WSPRINTF(szText,
               SIZEOF_AECHARS(128),
               szTextFormat,
               pMe->m_initialBetAmount,
               pMe->m_cash);
      break;

   default:
//      ASSERT_NOT_REACHABLE;
      break;
   }

   BlackJack_ReleaseControls(pMe);
   pMe->m_pIStatic = NULL;
   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectEraseScreen);
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStatic)) {
      return ;
   }
   pMe->m_pCurrentMenu = NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_SOFTKEYCTL,
                                            (void **)&pMe->m_pCurrentMenu)) {
      return ;
   }

   for( i=0; 0 != wButtons[i]; i++)
   {
      (void) IMENUCTL_AddItem(pMe->m_pCurrentMenu,
                              BLACKJACK_RES_FILE_LANGUAGE,
                              wButtons[i],
                              (uint16) wButtons[i],
                              NULL,
                              NULL);
   }

/*
   IMENUCTL_GetRect(pMe->m_pCurrentMenu, &rc);
   
   SETAEERECT(&rc,pMe->m_rectInfoScreen.x,pMe->m_rectInfoScreen.y,
                   pMe->m_rectInfoScreen.dx,pMe->m_rectInfoScreen.dy-rc.dy);

   ISTATIC_SetRect(pMe->m_pIStatic, &rc);
   */
   ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rectInfoScreen);
   ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL);

   (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                               BLACKJACK_RES_FILE_LANGUAGE,
                               titleID,
                               szBuff,
                               sizeof(szBuff));
   (void) ISTATIC_SetText(pMe->m_pIStatic,
                          szBuff,
                          szText,
                          AEE_FONT_BOLD,
                          AEE_FONT_NORMAL);
   IMENUCTL_SetActive(pMe->m_pCurrentMenu, TRUE);
   (void) ISTATIC_Redraw(pMe->m_pIStatic);
   (void) IMENUCTL_Redraw(pMe->m_pCurrentMenu);

   /*
   MEMSET(&bi, 0, sizeof(AEEPromptInfo));
   bi.pszRes   = BLACKJACK_RES_FILE_LANGUAGE;
   bi.wTitleID = titleID;
   bi.pText    = szText;
   bi.dwProps  = ST_CENTERTITLE;
   bi.pBtnIDs  = (const uint16 *) wButtons;
   bi.wDefBtn  = IDC_NEW_GAME;
   (void) ISHELL_Prompt(pMe->a.m_pIShell, &bi);
   */
}


/*=============================================================================
FUNCTION: BlackJack_CheckBetAmount

DESCRIPTION:
      Check the bet amount to make sure that:
      * The player has enough money to meet the table minimum
      * The player has enough money to make the current bet amount

      TODO:
      * Check to make sure that the current bet amount is >= table minimum
      * Check to make sure that the current bet amount is <= table maximum

      If one of the above conditions is not met, the appropriate state is set
      (which in turn will display a prompt with a message, etc.).

PARAMETERS:
   *pMe:
   bCheckAll: If true, perform all checks. Otherwise only check that the
              player can meet the table minimum.

RETURN VALUE:
   boolean:
              TRUE:           Bet amount is legal.
              FALSE:          Bet amount is not allowed/possible. A prompt
                              has been displayed with the player's option(s).

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_CheckBetAmount(BlackJackApp *pMe,
                                        boolean bCheckAll)
{
   boolean     bIsLegalBetAmount = TRUE;

   if (pMe->m_cash < pMe->m_tableMinimum) {
      bIsLegalBetAmount = FALSE;

      BlackJack_SetState(pMe, state_cashBelowBetMinimumScreen);
   } else if (bCheckAll && (pMe->m_initialBetAmount > pMe->m_cash)) {
      bIsLegalBetAmount = FALSE;

      BlackJack_SetState(pMe, state_betAmountGreaterThanCashScreen);
   }

   return bIsLegalBetAmount;
}


/*=============================================================================
FUNCTION: BlackJack_AddItemToChangeBetMenu

DESCRIPTION:
      Adds item to change bet menu (if appropriate) and returns whether or
      not the add occurred.

PARAMETERS:
   pMenu:
   bLastAddResult:
   nThreshold:
   nCashRemaining:
   nCurrentBet:
   nItemID:
   nClosestItemID:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_AddItemToChangeBetMenu(IMenuCtl * pMenu,
                                                boolean bLastAddResult,
                                                int nThreshold,
                                                int nCashRemaining,
                                                int nCurrentBet,
                                                uint16 nItemID,
                                                uint16 * nClosestItemID)
{
   boolean     addGood = bLastAddResult;

   if (bLastAddResult && pMenu && (nCashRemaining >= nThreshold)) {
      addGood =
         IMENUCTL_AddItem(pMenu,
                          BLACKJACK_RES_FILE_LANGUAGE,
                          nItemID,
                          nItemID,
                          NULL,
                          0);
      if (nCurrentBet >= nThreshold)
         *nClosestItemID = nItemID;
   }

   return addGood;
}


/*=============================================================================
FUNCTION: BlackJack_DisplayChangeBetMenu

DESCRIPTION: Displays the change bet menu.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DisplayChangeBetMenu(BlackJackApp *pMe)
{
   // Check to make sure that the user can at least meet the minimum bet for
   // the table. If not there would be no menu items. If
   // BlackJack_CheckBetAmount fails for this reason, then the user has
   // already been forced to start a new game.
   if (BlackJack_CheckBetAmount(pMe, kBetCheckJustMinimum)) {
      int         cashRemaining = pMe->m_cash;
      int         currentBet = pMe->m_initialBetAmount;
      uint16      closestCashValueID = IDC_BET_AMOUNT_10;
      boolean     initGood;

      // The temp menu should already be released, but if it's not then
      // release it here because it might not be an AEECLSID_MENUCTL
      if (pMe->m_pTempMenu) {
         IMENUCTL_Release(pMe->m_pTempMenu);
         pMe->m_pTempMenu = NULL;
      }
      //      Create the bet amount menu
      initGood =
         (boolean) (ISHELL_CreateInstance
                    (pMe->a.m_pIShell, AEECLSID_MENUCTL,
                     (void **) &pMe->m_pTempMenu) == SUCCESS);

      initGood =
         BlackJack_AddItemToChangeBetMenu(pMe->m_pTempMenu, initGood, 10,
                                          cashRemaining, currentBet,
                                          IDC_BET_AMOUNT_10,
                                          &closestCashValueID);

      initGood =
         BlackJack_AddItemToChangeBetMenu(pMe->m_pTempMenu, initGood, 20,
                                          cashRemaining, currentBet,
                                          IDC_BET_AMOUNT_20,
                                          &closestCashValueID);

      initGood =
         BlackJack_AddItemToChangeBetMenu(pMe->m_pTempMenu, initGood, 50,
                                          cashRemaining, currentBet,
                                          IDC_BET_AMOUNT_50,
                                          &closestCashValueID);

      initGood =
         BlackJack_AddItemToChangeBetMenu(pMe->m_pTempMenu, initGood, 100,
                                          cashRemaining, currentBet,
                                          IDC_BET_AMOUNT_100,
                                          &closestCashValueID);

      initGood =
         BlackJack_AddItemToChangeBetMenu(pMe->m_pTempMenu, initGood, 150,
                                          cashRemaining, currentBet,
                                          IDC_BET_AMOUNT_150,
                                          &closestCashValueID);

      initGood =
         BlackJack_AddItemToChangeBetMenu(pMe->m_pTempMenu, initGood, 200,
                                          cashRemaining, currentBet,
                                          IDC_BET_AMOUNT_200,
                                          &closestCashValueID);
      initGood = IMENUCTL_SetTitle( pMe->m_pTempMenu,
                         BLACKJACK_RES_FILE_LANGUAGE,
                         IDC_CHANGE_BET,
                         NULL);
      IMENUCTL_SetBottomBarType( pMe->m_pTempMenu, BTBAR_SELECT_BACK);

      if (initGood) {
//         pMe->m_pCurrentMenu = pMe->m_pTempMenu;
         IMENUCTL_SetSel(pMe->m_pTempMenu, closestCashValueID);
         IMENUCTL_SetActive(pMe->m_pTempMenu, TRUE);
         pMe->m_pCurrentMenu = pMe->m_pTempMenu;
         IDISPLAY_Update(pMe->a.m_pIDisplay);
      } else {
         //      Release the bet amount menu
         if (pMe->m_pTempMenu) {
            IMENUCTL_Release(pMe->m_pTempMenu);
            pMe->m_pTempMenu = NULL;
         }

         BlackJack_SetState(pMe, state_betweenHands);
      }
   }
}


/*=============================================================================
FUNCTION: BlackJack_ChangeBetAmount

DESCRIPTION:
      Handle the user's choice of the new bet amount. For now this is
      just what menu item the user chose.

PARAMETERS:
   *pMe:
   wParam:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ChangeBetAmount(BlackJackApp *pMe,
                                      uint16 wParam)
{
   pMe->m_pCurrentMenu = NULL;

   // We're done with the bet amount menu, free the memory
   if (pMe->m_pTempMenu) {
      IMENUCTL_Release(pMe->m_pTempMenu);
      pMe->m_pTempMenu = NULL;
   }

   // Detemine the new bet amount
   switch (wParam) {
   case IDC_BET_AMOUNT_10:
      pMe->m_initialBetAmount = 10;
      break;

   case IDC_BET_AMOUNT_20:
      pMe->m_initialBetAmount = 20;
      break;

   case IDC_BET_AMOUNT_50:
      pMe->m_initialBetAmount = 50;
      break;

   case IDC_BET_AMOUNT_100:
      pMe->m_initialBetAmount = 100;
      break;

   case IDC_BET_AMOUNT_150:
      pMe->m_initialBetAmount = 150;
      break;

   case IDC_BET_AMOUNT_200:
   default:
      pMe->m_initialBetAmount = 200;
      break;
   }

   // If the bet amount checks out, show the between hands info screen. If it
   // didn't check out, then BlackJack_CheckBetAmount would have displayed
   // a prompt followed by the chose bet amount menu.
   if (BlackJack_CheckBetAmount(pMe, kBetCheckAll)) {
      BlackJack_SetState(pMe, state_betweenHands);
   }
}


/*=============================================================================
FUNCTION: BlackJack_DisplayInsuranceOffer

DESCRIPTION:
      Display insurance offer in a static below the dealer's cards with
      a menu that allows the player to choose yes or no.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DisplayInsuranceOffer(BlackJackApp *pMe)
{
   boolean     initGood;

   // The temp menu should already be released, but if it's not then
   // release it here because it might not be an AEECLSID_SOFTKEYCTL
   if (pMe->m_pTempMenu) {
      IMENUCTL_Release(pMe->m_pTempMenu);
      pMe->m_pTempMenu = NULL;
   }
   //      Create the insurance choice menu
   initGood =
      (boolean) (ISHELL_CreateInstance
                 (pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL,
                  (void **) &pMe->m_pTempMenu) == SUCCESS);

   if (initGood)
      initGood =
         IMENUCTL_AddItem(pMe->m_pTempMenu, BLACKJACK_RES_FILE_LANGUAGE,
                          IDC_WANT_INSURANCE, IDC_WANT_INSURANCE, NULL, 0);
   if (initGood)
      initGood =
         IMENUCTL_AddItem(pMe->m_pTempMenu, BLACKJACK_RES_FILE_LANGUAGE,
                          IDC_NO_INSURANCE, IDC_NO_INSURANCE, NULL, 0);

   if (initGood)
      initGood =
         BlackJack_DisplayInsuranceStatic(pMe, IDS_INSURANCE_OFFER_TITLE,
                                          IDS_INSURANCE_OFFER_MSG,
                                          kLeaveSpaceForMenu);

   if (initGood) {
//      pMe->m_pCurrentMenu = pMe->m_pTempMenu;
      IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectMenu);
      IMENUCTL_SetSel(pMe->m_pTempMenu, IDC_NO_INSURANCE);
      IMENUCTL_SetActive(pMe->m_pTempMenu, TRUE);
      pMe->m_pCurrentMenu = pMe->m_pTempMenu;

      IDISPLAY_Update(pMe->a.m_pIDisplay);
   } else {
      //      Release the insurance choice menu
      if (pMe->m_pTempMenu) {
         IMENUCTL_Release(pMe->m_pTempMenu);
         pMe->m_pTempMenu = NULL;
      }
      //      Something failed, continue as if the player declined insurance
      BlackJack_HandleInsuranceResponse(pMe, IDC_NO_INSURANCE);
   }
}


/*=============================================================================
FUNCTION: BlackJack_DisplayInsuranceStatic

DESCRIPTION:
      Display insurance related message in a static leaving space for a menu
      below the static if indicated.

PARAMETERS:
   *pMe:
   nTitleResID: Resource ID for static title string
   nMessageFormatResID: Resource ID for static text format
   bLeaveSpaceForMenu: TRUE if the static's rect should leave
                       space for a menu below it

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean BlackJack_DisplayInsuranceStatic(BlackJackApp *pMe,
                                                uint16 nTitleResID,
                                                uint16 nMessageFormatResID,
                                                boolean bLeaveSpaceForMenu)
{
   boolean     initGood = TRUE;

   //      Display the insurance message in our static
   if (pMe->m_pIStatic == NULL)
      initGood =
         (boolean) (ISHELL_CreateInstance
                    (pMe->a.m_pIShell, AEECLSID_STATIC,
                     (void **) &pMe->m_pIStatic) == SUCCESS);

   if (initGood) {
      AEERect     rectInsuranceMessage;
      int         dealerCardAreaHeight,
                  rectHeight;
      AECHAR      szInsuranceTitle[16];
      AECHAR     *szInsuranceTextFormat;
      AECHAR     *szInsuranceText;

      // Use 128 characters for szInsuranceTextFormat
      szInsuranceTextFormat = pMe->m_szBuffer;
      szInsuranceTextFormat[0] = 0;

      // Use 128 characters for szInsuranceText after szInsuranceTextFormat
      szInsuranceText = &pMe->m_szBuffer[128];
      szInsuranceText[0] = 0;

      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  nTitleResID,
                                  szInsuranceTitle,
                                  sizeof(szInsuranceTitle));

      (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  BLACKJACK_RES_FILE_LANGUAGE,
                                  nMessageFormatResID,
                                  szInsuranceTextFormat,
                                  SIZEOF_AECHARS(128));

      if (nMessageFormatResID == IDS_INSURANCE_OFFER_MSG) {
         WSPRINTF(szInsuranceText,
                  SIZEOF_AECHARS(128),
                  szInsuranceTextFormat,
                  pMe->m_initialBetAmount / 2,
                  pMe->m_initialBetAmount);
      } else {
         WSPRINTF(szInsuranceText,
                  SIZEOF_AECHARS(128),
                  szInsuranceTextFormat,
                  pMe->m_initialBetAmount / 2);
      }

      // Set up the rect for insurance static to be the entire playable
      // area below the dealer's cards
      dealerCardAreaHeight = pMe->m_cardImageHeight + kVerticalSpaceBetweenCards;
      rectHeight = pMe->m_rectScreen.dy - dealerCardAreaHeight;

      if (bLeaveSpaceForMenu)
         rectHeight -= pMe->m_rectMenu.dy;
      else
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectMenu);

      SETAEERECT(&rectInsuranceMessage, pMe->m_rectInfoScreen.x,
                 dealerCardAreaHeight+kSpaceBetweenCardAndEdge, pMe->m_rectInfoScreen.dx, rectHeight);

      // Erase the area that we're about to draw our insurance static
      // to be sure
      IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rectInsuranceMessage);

      ISTATIC_SetRect(pMe->m_pIStatic, &rectInsuranceMessage);
      ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE|ST_NOSCROLL);
      (void) ISTATIC_SetText(pMe->m_pIStatic,
                             szInsuranceTitle,
                             szInsuranceText,
                             AEE_FONT_BOLD,
                             AEE_FONT_NORMAL);
      (void) ISTATIC_Redraw(pMe->m_pIStatic);
   }

   return initGood;
}


/*=============================================================================
FUNCTION: BlackJack_HandleInsuranceResponse

DESCRIPTION: Handles player's response to insurance offer

PARAMETERS:
   *pMe:
   wParam: Players response to insurance offer -
                            either IDC_WANT_INSURANCE or IDC_NO_INSURANCE

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_HandleInsuranceResponse(BlackJackApp *pMe,
                                              uint16 wParam)
{
   boolean     displayInsuranceResultFailed = FALSE;

   //      Release the memory for the insurance offer menu
   if (pMe->m_pTempMenu) {
      IMENUCTL_Release(pMe->m_pTempMenu);
      pMe->m_pTempMenu = NULL;
   }

   if (wParam == IDC_WANT_INSURANCE) {
      if (pMe->m_dealersHandValue == kBlackJackValue) {
         // Mark that the player had an insurance bet so that we can show the
         // appropriate hand result message and deal with insurance winnings.
         // We don't modify the cash amount here because we don't want the
         // user to see the money amount change before we get to the between
         // hands info screen (in particular if insurance is counteracting
         // the players loss the amount of money should just stay the same,
         // rather than going up here and down again later).
         SetOptionOn(pMe, playopt_insuranceBet);
      } else {
         // Dealer does not have BlackJack - player loses the insurance bet,
         // which is half the intial bet. Do this immediately because we are
         // not going to the between hands info screen yet.
         pMe->m_cash -= pMe->m_initialBetAmount / 2;

         if (BlackJack_DisplayInsuranceStatic
             (pMe, IDS_INSURANCE_RESULT_TITLE, IDS_INSURANCE_RESULT_MSG,
              kDontLeaveSpaceForMenu)) {
            // Draw the current hands, showing only the dealers cards
            BlackJack_DrawHands(pMe, kDealerNotPlaying, kDontDrawPlayersHand);

            IDISPLAY_Update(pMe->a.m_pIDisplay);
            BlackJack_SetState(pMe, state_pausingToShowInsuranceResult);
         } else { //      Something failed
            displayInsuranceResultFailed = TRUE;
         }
      }
   }

   if ((wParam != IDC_WANT_INSURANCE) ||
       (pMe->m_dealersHandValue == kBlackJackValue) ||
        displayInsuranceResultFailed) {
      // Release the memory for the insurance offer message
      if (pMe->m_pIStatic) {
         ISTATIC_Release(pMe->m_pIStatic);
         pMe->m_pIStatic = NULL;
      }
      // Draw the current hands, showing both of the dealers cards
      // only if the dealer has BlackJack
      BlackJack_DrawHands(pMe,
                          (boolean) (pMe->m_dealersHandValue ==
                                     kBlackJackValue), kDrawPlayersHand);

      // Draw the cash info
      BlackJack_DrawCashInfo(pMe);

      // Check for BlackJack and set the next state appropriately
      BlackJack_CheckForBlackJack(pMe);

      IDISPLAY_Update(pMe->a.m_pIDisplay);
   }
}


/*=============================================================================
FUNCTION: BlackJack_ShowInsuranceResultMessageTimer

DESCRIPTION:
      Delay before continuing with play so that the player can see the
      insurance result message.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ShowInsuranceResultMessageTimer(BlackJackApp *pMe)
{
   //  Release the memory for the insurance result message
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }
   // Draw the current hands, showing the dealers hand only if the
   // dealer has BlackJack

   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectEraseScreen);
   
   BlackJack_DrawHands(pMe,
                       (boolean) (pMe->m_dealersHandValue ==
                                  kBlackJackValue), kDrawPlayersHand);

   // Draw the cash info
   BlackJack_DrawCashInfo(pMe);

   // Check for BlackJack and set the next state appropriately
   BlackJack_CheckForBlackJack(pMe);

   IDISPLAY_Update(pMe->a.m_pIDisplay);
}

/*=============================================================================
FUNCTION: BlackJack_ShuffleCardMessageDoneTimer

DESCRIPTION:
      Delay before continuing with the deal so that the player can see the
      shuffle cards message.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_ShuffleCardMessageDoneTimer(BlackJackApp *pMe)
{
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }

   BlackJack_InitialDeal(pMe);
}


/*=============================================================================
FUNCTION: BlackJack_InitialDeal

DESCRIPTION:
      Deal out the initial cards detecting if the player has BlackJack so
      that we can skip the dealer playing.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_InitialDeal(BlackJackApp *pMe)
{
   CardKindEnum dealersFaceUpCardKind;

   //      All options off initially
   pMe->m_playOptionFlags = 0;

   // The bet amount starts out at the initial bet amount
   pMe->m_handBetAmount = pMe->m_initialBetAmount;

   // Deal two cards to the player
   pMe->m_playersHand[0] = DealNextCard(pMe);
   pMe->m_playersHand[1] = DealNextCard(pMe);
   pMe->m_playersHandSize = 2;

   // Deal two cards to the dealer
   pMe->m_dealersHand[0] = DealNextCard(pMe);
   pMe->m_dealersHand[1] = DealNextCard(pMe);
   pMe->m_dealersHandSize = 2;

   // Calculate the hand values so that we can detect BlackJack
   pMe->m_playersHandValue =
      BlackJack_CalculateHandValue(pMe->m_playersHand,
                                   pMe->m_playersHandSize, kBlackJackAllowed);
   pMe->m_dealersHandValue =
      BlackJack_CalculateHandValue(pMe->m_dealersHand,
                                   pMe->m_dealersHandSize, kBlackJackAllowed);

   dealersFaceUpCardKind = CardIDToKind(pMe->m_dealersHand[1]);

   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &pMe->m_rectEraseScreen);

   if (dealersFaceUpCardKind == card_Ace) {
      // Offer insurance without checking to see if dealer actually
      // has BlackJack
      BlackJack_DrawHands(pMe, kDealerNotPlaying, kDontDrawPlayersHand);
      BlackJack_SetState(pMe, state_showingInsuranceOffer);
   } else {
      // Draw the current hands, showing the dealers hand only if the
      // dealer has BlackJack
      BlackJack_DrawHands(pMe,
                          (boolean) (pMe->m_dealersHandValue ==
                                     kBlackJackValue), kDrawPlayersHand);

      // No need for insurance, draw the cash info
      BlackJack_DrawCashInfo(pMe);

      // Check for BlackJack and set the next state appropriately
      BlackJack_CheckForBlackJack(pMe);
   }

   IDISPLAY_Update(pMe->a.m_pIDisplay);
}


/*=============================================================================
FUNCTION: BlackJack_CheckForBlackJack

DESCRIPTION:
      Checks for BlackJack and sets the next state appropriately.
      If there's BlackJack we pause then show the winner.
      If there's no BlackJack, we just continue with the player playing.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_CheckForBlackJack(BlackJackApp *pMe)
{
   if ((pMe->m_playersHandValue == kBlackJackValue)
       || (pMe->m_dealersHandValue == kBlackJackValue)) {
      //      Someone has BlackJack
      BlackJack_SetState(pMe,
                         state_pausingToShowHandBeforeDeterminingWinner);
   } else {
      // No BlackJack, just proceed with play

      // If the player's initial hand value is in the correct range and
      // the player has enough cash left, then enable the double down option.
      if ((pMe->m_playersHandValue >= kDoubleDownLowerLimit) &&
          (pMe->m_playersHandValue <= kDoubleDownUpperLimit) &&
          ((pMe->m_handBetAmount * 2) <= pMe->m_cash)) {
         SetOptionOn(pMe, playopt_doubleDownAllowed);
      } else {
         SetOptionOff(pMe, playopt_doubleDownAllowed);
      }

      BlackJack_SetState(pMe, state_playerPlayingFirstTwoCards);
   }
}


/*=============================================================================
FUNCTION: BlackJack_Hit

DESCRIPTION:
      Handle the IDC_HIT command by hitting the player with another card
      off the deck.

PARAMETERS:
   *pMe:
   doublingDown: Is the player doubling down?

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_Hit(BlackJackApp *pMe,
                          boolean doublingDown)
{
   boolean     bIsDealerPlaying;

   //      Deal next card and recalculate the player's hand value
   pMe->m_playersHand[pMe->m_playersHandSize++] = DealNextCard(pMe);
   pMe->m_playersHandValue =
      BlackJack_CalculateHandValue(pMe->m_playersHand,
                                   pMe->m_playersHandSize, kBlackJackAllowed);

   if (doublingDown) {
      //      Double the bet amount and redraw the cash info
      pMe->m_handBetAmount *= 2;
      BlackJack_DrawCashInfo(pMe);
   }

   if (pMe->m_playersHandValue > kHandValueMax) {
      //      The player busted - there's no need for the dealer to play
      BlackJack_SetState(pMe, state_pausingToShowHandBeforeDeterminingWinner);
      bIsDealerPlaying = kDealerNotPlaying;
   } else if (doublingDown) {
      //      The player doubled down - the dealer plays now
      BlackJack_DealerNextAction(pMe);
      bIsDealerPlaying = kDealerPlaying;
   } else {
      //      No splitting or doubling down after the first hit
      BlackJack_SetState(pMe, state_playerPlaying);

      //      Player is still playing
      bIsDealerPlaying = kDealerNotPlaying;
   }

   //      Draw the hands showing the dealers information if appropriate
   BlackJack_DrawHands(pMe, bIsDealerPlaying, kDrawPlayersHand);

   IDISPLAY_Update(pMe->a.m_pIDisplay);
}


/*=============================================================================
FUNCTION: BlackJack_Stand

DESCRIPTION: Handle the IDC_STAND command by starting the dealers play.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_Stand(BlackJackApp *pMe)
{
   pMe->m_dealersHandValue =
      BlackJack_CalculateHandValue(pMe->m_dealersHand,
                                   pMe->m_dealersHandSize, kBlackJackAllowed);

   //      Draw hands showing the dealers hidden card
   BlackJack_DrawHands(pMe, kDealerPlaying, kDrawPlayersHand);

   //      Dealer plays or we determine winner
   BlackJack_DealerNextAction(pMe);

   IDISPLAY_Update(pMe->a.m_pIDisplay);
}


/*=============================================================================
FUNCTION: BlackJack_DealerNextAction

DESCRIPTION:
   Set up a timer for either the next move or to determine the results.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DealerNextAction(BlackJackApp *pMe)
{
   // The dealer has very strict rules - hit until the predetermined value
   // is reached
   if (pMe->m_dealersHandValue <= kDealerHitMax)
      BlackJack_SetState(pMe, state_dealerPlaying);
   else
      BlackJack_SetState(pMe, state_pausingToShowHandBeforeDeterminingWinner);
}


/*=============================================================================
FUNCTION: BlackJack_DealerPlayTimer

DESCRIPTION:
   Play one card at a time with a delay between each so that the player
   can see what the dealer is doing.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DealerPlayTimer(BlackJackApp *pMe)
{
   // We know that the dealer needs to hit because we checked the hand value
   // before setting up this timer routine
   pMe->m_dealersHand[pMe->m_dealersHandSize++] = DealNextCard(pMe);
   pMe->m_dealersHandValue =
      BlackJack_CalculateHandValue(pMe->m_dealersHand,
                                   pMe->m_dealersHandSize, kBlackJackAllowed);
   BlackJack_DrawHands(pMe, kDealerPlaying, kDrawPlayersHand);
   IDISPLAY_Update(pMe->a.m_pIDisplay);

   //      Hit again or determine winner
   BlackJack_DealerNextAction(pMe);
}


/*=============================================================================
FUNCTION: BlackJack_DetermineWinnerTimer

DESCRIPTION:
      Delay before determining the winner so that the player can see what
      has happened.

PARAMETERS:
   *pMe:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void BlackJack_DetermineWinnerTimer(BlackJackApp *pMe)
{
   uint8       dealersHandRankingValue,
               playersHandRankingValue;

   dealersHandRankingValue = pMe->m_dealersHandValue;
   if ((dealersHandRankingValue != kBlackJackValue) &&
       (dealersHandRankingValue > kHandValueMax)) {
      dealersHandRankingValue = 0;
   }

   playersHandRankingValue = pMe->m_playersHandValue;
   if ( (playersHandRankingValue != kBlackJackValue) &&
        (playersHandRankingValue > kHandValueMax) ) {
      playersHandRankingValue = 0;
   }

   if (dealersHandRankingValue < playersHandRankingValue) {
      //  BlackJack - the player gets an extra 50% rounded down to nearest
      //  dollar amount (the casino has no chips below 1 dollar, it pays to
      //  play even dollar amounts :-)
      if (playersHandRankingValue == kBlackJackValue) {
         pMe->m_handBetAmount += pMe->m_handBetAmount / 2;
      }

      //      Player wins bet
      pMe->m_cash += pMe->m_handBetAmount;

      //      Choose correct winning hand title
      if (playersHandRankingValue == kBlackJackValue)
         pMe->m_nInfoTitleResID = IDS_PLAYER_BLACKJACK_TITLE;
      else if (dealersHandRankingValue == 0)
         pMe->m_nInfoTitleResID = IDS_DEALER_BUSTED_TITLE;
      else
         pMe->m_nInfoTitleResID = IDS_PLAYER_WON_TITLE;

      //      Money result string
      pMe->m_nMoneyResultResID = IDS_PLAYER_WINNINGS;
   } else if (dealersHandRankingValue > playersHandRankingValue) {
      //      Player loses bet, unless the player chose insurance and the
      //      dealer had BlackJack
      if ((dealersHandRankingValue != kBlackJackValue) ||
           !IsOptionOn(pMe, playopt_insuranceBet)) {
         pMe->m_cash -= pMe->m_handBetAmount;
      }

      //      Choose correct losing hand title
      if (dealersHandRankingValue == kBlackJackValue) {
         pMe->m_nInfoTitleResID = IDS_DEALER_BLACKJACK_TITLE;
      } else if (playersHandRankingValue == 0) {
         pMe->m_nInfoTitleResID = IDS_PLAYER_BUSTED_TITLE;
      } else {
         pMe->m_nInfoTitleResID = IDS_DEALER_WON_TITLE;
      }

      //      Determine correct money result string
      if ((dealersHandRankingValue == kBlackJackValue) &&
            IsOptionOn(pMe, playopt_insuranceBet)) {
         pMe->m_nMoneyResultResID = IDS_BJ_BUT_INSURANCE_NO_WINNINGS;

         //      The insurance bet counteracted the player's loss due to the
         //      dealer's BlackJack.  Tell the player that it's break even.
         pMe->m_handBetAmount = 0;
      } else {
         pMe->m_nMoneyResultResID = IDS_PLAYER_LOSINGS;
      }

   } else {   // Push no money won or lost
      pMe->m_nInfoTitleResID = IDS_PUSH_TITLE;

      // Choose correct money result string
      if (dealersHandRankingValue == kBlackJackValue) {
         if (IsOptionOn(pMe, playopt_insuranceBet)) {
            // Dealer had BlackJack, player wins twice the insurance bet
            // (since the insurance bet is half the intial bet - this just
            // amounts to the intial bet)
            pMe->m_cash += pMe->m_initialBetAmount;

            pMe->m_nMoneyResultResID = IDS_BJ_PUSH_BUT_INSURANCE_WINNINGS;
            pMe->m_handBetAmount = pMe->m_initialBetAmount;
         } else {
            pMe->m_nMoneyResultResID = IDS_BJ_PUSH_NO_WINNINGS;
            pMe->m_handBetAmount = 0;
         }
      } else {
         pMe->m_nMoneyResultResID = IDS_PUSH_NO_WINNINGS;
         pMe->m_handBetAmount = 0;
      }
   }

   BlackJack_SetState(pMe, state_betweenHands);
}


/*=============================================================================
FUNCTION: Util_DrawCard

DESCRIPTION:
      Draws an individual card by drawing the card kind/value on the top and
      then the card suit on the bottom.

PARAMETERS:
   pCardImages: Card images
   card: Card ID to draw
   xPos: x position of upper left of card
   yPos:y position of upper left of card
   cardImageHeight: Height of a single card image
   bCardIsHidden: TRUE if card is hidden - i.e. special image is drawn
                                                instead of value
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Util_DrawCard(IImage * pCardImages,
                          CardIDType card,
                          int xPos,
                          int yPos,
                          uint16 cardImageHeight,
                          boolean bCardIsHidden)
{
   int          cardSuit = card / kNumberIdentities;
   CardKindEnum cardKind = (CardKindEnum) (card % kNumberIdentities);
   int          imageIndex;
   int          bottomHalfYPos = yPos + (cardImageHeight / 2);

   imageIndex = bCardIsHidden ? kHiddenCardTopImageIndex :
                                (int) cardKind + kFirstCardKindImageIndex;

   IIMAGE_DrawFrame(pCardImages, imageIndex, xPos, yPos);

   imageIndex = bCardIsHidden ? kHiddenCardBottomImageIndex : cardSuit;

   IIMAGE_DrawFrame(pCardImages, imageIndex, xPos, bottomHalfYPos);
}


/*=============================================================================
FUNCTION: Util_DrawCards

DESCRIPTION:
      Draws cards with smart spacing - no overlap if possible, some overlap
      if necessary, all overlap if necessary, left card(s) dropped as a
      last resort.

PARAMETERS:
   pCardImages: Card images
   cardsArr[]: Cards to draw
   numberOfCards: Number of cards to draw
   firstNonHiddenCard: First card to show front of (usually 0 or 1)
   in_xPos: x position of upper left of first card
   yPos: y position of upper left of first card
   widthAvailable: Amount of width available - used to calculate need to
                                               overlap or drop card
   cardImageHeight: Height of a single card image
   cardImageWidth: Width of a single card image
   cardImagePreferredSpacing: Preferred spacing between card images
                              including the width of the card
   cardImageOverlappedSpacing: Spacing between card images with cards to
                               the right drawn on top of part of card to
                               the left

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Util_DrawCards(IImage * pCardImages,
                           CardIDType cardsArr[],
                           int numberOfCards,
                           int firstNonHiddenCard,
                           int in_xPos,
                           int yPos,
                           int widthAvailable,
                           uint16 cardImageHeight,
                           uint16 cardImageWidth,
                           uint16 cardImagePreferredSpacing,
                           uint16 cardImageOverlappedSpacing)
{
   CardIDType  card;
   int         i;
   int         xPos = in_xPos;
   int         lastCardToOverlap = 0;
   int         widthUsed = 0;

   if (numberOfCards <= 0)
      return;

   // The width used is the preferred spacing for each card except the
   // last card plus the width of the last card
   widthUsed = (numberOfCards - 1) * cardImagePreferredSpacing +
                                                               cardImageWidth;

   if (widthUsed > widthAvailable) {
      //      We're too wide - overlap cards
      int         firstCardToDraw = 0;
      int         widthOverflow = widthUsed - widthAvailable;
      int         widthSavedByOneOverlap =
         cardImagePreferredSpacing - cardImageOverlappedSpacing;

      lastCardToOverlap =
         (widthOverflow + widthSavedByOneOverlap - 1) / widthSavedByOneOverlap;

      // We can't save space by overlapping the last card, which gets its
      // full width displayed (it can overlap the card to its left, but by
      // definition cannot be overlapped itself). Therefore if the
      // lastCardToOverlap is the equal to the number of cards  we're
      // already too wide.
      if (lastCardToOverlap >= numberOfCards) {
         // We're still too wide, drop cards off the left edge
         int maxCardsInAvailableWidth = ((widthAvailable - cardImageWidth) /
                                          cardImageOverlappedSpacing) + 1;

         firstCardToDraw = numberOfCards - maxCardsInAvailableWidth;
         lastCardToOverlap = numberOfCards;

         // Recalculate the last card to overlap, because now that we have
         // dropped one or more cards we may have room to not overlap the
         // last card with the second to last card.
         widthUsed = ((numberOfCards - firstCardToDraw) - 1) *
                                   cardImagePreferredSpacing + cardImageWidth;
         widthOverflow = widthUsed - widthAvailable;
         lastCardToOverlap =
            ((widthOverflow + widthSavedByOneOverlap -
              1) / widthSavedByOneOverlap) + firstCardToDraw;
      }
      //      Draw overlapped cards
      for (i = firstCardToDraw; i < lastCardToOverlap; i++) {
         card = cardsArr[i];

         Util_DrawCard(pCardImages, card, xPos, yPos, cardImageHeight,
                       (boolean) (i < firstNonHiddenCard));

         xPos += cardImageOverlappedSpacing;
      }
   }
   // Draw remaining cards (if any) with normal spacing
   for (i = lastCardToOverlap; i < numberOfCards; i++) {
      card = cardsArr[i];

      Util_DrawCard(pCardImages, card, xPos, yPos, cardImageHeight,
                    (boolean) (i < firstNonHiddenCard));

      xPos += cardImagePreferredSpacing;
   }
}


/*=============================================================================
FUNCTION: Util_AddString

DESCRIPTION: Appends a new line and a string onto an existing string

PARAMETERS:
   pDest: Destination string
   pText:  String to append

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Util_AddString(AECHAR * pDest,
                           AECHAR * pText)
{
   WSTRCAT(pDest, (AECHAR *) "\n");
   if (pText)
      WSTRCAT(pDest, pText);
}
static void BlackJack_ReleaseControls(BlackJackApp *pMe)
{
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }
   if (pMe->m_pCurrentMenu) {
      IMENUCTL_Release(pMe->m_pCurrentMenu);
      pMe->m_pCurrentMenu = NULL;
   }
}

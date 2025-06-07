/* ************************************************************************
 *
 * main part
 *
 * (c) 2012-2024 by Markus Reschke
 * based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 * include header files
 */

/* local includes */
#include "config.h"             /* global configuration */
#include "common.h"             /* common header file */
#include "variables.h"          /* global variables */
#include "functions.h"          /* external functions */
#include "colors.h"             /* color definitions */
// Aggiunto per compatibilità con l'ambiente Arduino/ESP32, anche se spesso incluso da altre parti
#include <Arduino.h>


/*
 * local variables
 */

/* program control */
#if CYCLE_MAX < 255
uint8_t         MissedParts;           /* counter for failed/missed components */
#endif

// Dichiarazione esterna di UI, che verrà definita in display.h o .ino
extern struct UI_Display UI;


/* ************************************************************************
 * output components and errors
 * ************************************************************************ */


/*
 * get pin designator for specific probe ID
 *
 * requires:
 * - probe ID [0-2]
 *
 * returns:
 * - pin designator
 *
 * NOTA: Questa funzione verrà spostata in display.cpp ma lasciata qui per ora
 * per mantenere la compilabilità durante la transizione.
 */
uint8_t Get_SemiPinDesignator(uint8_t Probe)
{
  uint8_t           Char;          /* designator / return value */

  /* looking for matching probe ID */
  if (Probe == Semi.A)             /* matches pin A */
  {
    Char = Semi.DesA;              /* designator for pin A */
  }
  else if (Probe == Semi.B)        /* matches pin B */
  {
    Char = Semi.DesB;              /* designator for pin B */
  }
  else                             /* matches pin C */
  {
    Char = Semi.DesC;              /* designator for pin C */
  }

  return Char;
}



#if ! defined (UI_NO_TEXTPINOUT) || defined (SW_ENCODER)

/*
 * show pinout for semiconductors
 * - displays: 123=abc
 */

void Show_SemiPinout(void)
{
  uint8_t           n;             /* counter */

  /* display: 123 */
  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    Display_ProbeNumber(n);        /* display probe ID */
  }

  /* display: = */
  Display_Char('=');

  /* display pin designators */
  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    Display_SemiPinDesignator(n);  /* display pin designator */
  }
}

#endif



/*
 * show simple pinout
 * - displays: 1:a 2:b 3:c 
 *
 * required:
 * - DesX: designators for probes #1, #2 and #3
 * 0 -> not displayed
 */

void Show_SimplePinout(uint8_t Des1, uint8_t Des2, uint8_t Des3)
{
  uint8_t           n;     /* counter */
  unsigned char     Des[3];    /* component pin designators */
  #ifdef UI_PROBE_COLORS
  uint16_t          Color;   /* color value */

  Color = UI.PenColor;             /* save current color */
  #endif

  /* copy probe pin designators */
  Des[0] = Des1;
  Des[1] = Des2;
  Des[2] = Des3;

  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    if (Des[n] != 0)               /* display this one */
    {
      Display_ProbeNumber(n);
      Display_Colon();

      #ifdef UI_PROBE_COLORS
      UI.PenColor = ProbeColors[n];      /* set probe color */
      #endif

      Display_Char(Des[n]);

      #ifdef UI_PROBE_COLORS
      UI.PenColor = Color;               /* restore old color */
      #endif

      Display_Space();
    }
  }
}



#ifdef FUNC_EVALUE

/*
 * show E series norm values
 *
 * requires:
 * - Value: unsigned value
 * - Scale: exponent/multiplier (* 10^n)
 * - ESeries: E6-192
 * - Tolerance: in 0.1%
 * - Unit: unit character
 */

void Show_ENormValues(uint32_t Value, int8_t Scale, uint8_t ESeries, uint8_t Tolerance, unsigned char Unit)
{
  uint8_t           n;             /* number of norm values (1 or 2) */
  uint8_t           Pos;           /* char x position */
  uint8_t           Temp;          /* temporary value */


  /*
   * get E series norm values
   * - 1st: Semi.I_value, Semi.I_scale
   * - 2nd: Semi.C_value, Semi.C_scale
   */

  n = GetENormValue(Value, Scale, ESeries, Tolerance);


  /*
   * show E series and tolerance
   */

  Display_NextLine();                      /* move to next line */

  /* display E series */
  Display_Char('E');                       /* display: E */
  Display_FullValue(ESeries, 0 , 0);       /* display E series */

  Display_Space();                         /* display: " " */

  /* display tolerance */
  Temp = Tolerance;                        /* copy tolerance */
  Pos = 0;                                 /* reset decimal places */

  if (Temp < 10)                           /* < 1% */
  {
    Pos = 1;                               /* one decimal place */
  }
  Temp /= 10;                              /* scale to 1 */

  Display_FullValue(Temp, Pos, '%');       /* display tolerance */

  Display_Space();                         /* display: " " */


  /*
   * show norm values
   */

  if (n)                             /* got norm value(s) */
  {
    Pos = UI.CharPos_X;      /* get current char position */

    /* display first norm value */
    Display_EValue(Semi.I_value, Semi.I_scale, Unit);

    if (n == 2)              /* got two norm values */
    {
      /* move to next line at same position (after E series) */
      Display_NextLine();
      LCD_CharPos(Pos, UI.CharPos_Y);

      /* display second norm value */
      Display_EValue(Semi.C_value, Semi.C_scale, Unit);
    }
  }
  else                               /* no norm value */
  {
    Display_Minus();           /* display: - */
  }
}

#endif



#ifdef FUNC_COLORCODE

/*
 * show E series norm values as color-code
 *
 * requires:
 * - Value: unsigned value
 * - Scale: exponent/multiplier (* 10^n)
 * - ESeries: E6-192
 * - Tolerance: in 0.1%
 * - TolBand: color of tolerance band
 */

void Show_ENormCodes(uint32_t Value, int8_t Scale, uint8_t ESeries, uint8_t Tolerance, uint16_t TolBand)
{
  uint8_t           n;             /* number of norm values (1 or 2) */
  uint8_t           Pos;           /* char x position */

  /*
   * tolerance band
   * - resistor
   * 20%  10%    5%    2%  1%    0.5%  0.25% 0.1%    0.05%
   * none silver gold red brown green blue  violet grey
   * - inductor
   * 20%   10%    5%    4%    3%    2%  1%    0.5%  0.25% 0.1%    0.05%
   * black silver gold yellow orange red brown green blue  violet grey
   * - caps: many different schemas
   *
   * multiplier reference
   * - R:    0 (10^0)    -> 1 Ohms
   * C: -12 (10^-12) -> 1 pF
   * L:  -6 (10^-6)  -> 1 µH
   * - ref_scale as function argument?
   * scale = scale - ref_scale
   */

  /*
   * get E series norm values
   * - 1st: Semi.I_value, Semi.I_scale
   * - 2nd: Semi.C_value, Semi.C_scale
   */

  n = GetENormValue(Value, Scale, ESeries, Tolerance);


  /*
   * show E series
   */

  Display_NextLine();                      /* move to next line */

  /* display E series */
  Display_Char('E');                       /* display: E */
  Display_FullValue(ESeries, 0 , 0);       /* display E series */

  Display_Space();                         /* display: " " */


  /*
   * show color-codes of norm values
   */

  if (n)                             /* got norm value(s) */
  {
    Pos = UI.CharPos_X;      /* get current char position */

    /* display color-code of first norm value */
    Display_ColorCode(Semi.I_value, Semi.I_scale, TolBand);

    if (n == 2)              /* got two norm values */
    {
      /* move to next line at same position (after E series) */
      Display_NextLine();
      LCD_CharPos(Pos, UI.CharPos_Y);

      /* display color-code of second norm value */
      Display_ColorCode(Semi.C_value, Semi.C_scale, TolBand);
    }
  }
  else                               /* no norm value */
  {
    Display_Minus();           /* display: - */
  }

/* Il blocco seguente era originariamente racchiuso in #if 0. Lo lascio così. */
/*
#if 0
  // color testing
  uint16_t Color;

  Display_NextLine();

  // first 5 colors plus gold
  n = 0;
  while (n < 5)
  {
    Color = DATA_read_word((uint16_t *)&ColorCode_table[n]); // DATA_read_word non è standard C/C++
    LCD_Band(Color, ALIGN_LEFT);
    n++;
  }
  LCD_Band(COLOR_CODE_GOLD, ALIGN_RIGHT);

  Display_NextLine();

  // last 5 colors plus silver
  while (n < 10)
  {
    Color = DATA_read_word((uint16_t *)&ColorCode_table[n]); // DATA_read_word non è standard C/C++
    LCD_Band(Color, ALIGN_LEFT);
    n++;
  }
  LCD_Band(COLOR_CODE_SILVER, ALIGN_RIGHT);

  TestKey(0, CURSOR_BLINK);
#endif
*/
}

#endif



#ifdef FUNC_EIA96

/*
 * show E series norm values as EIA-96 code
 * - implies E96 1%
 *
 * requires:
 * - Value: unsigned value
 * - Scale: exponent/multiplier (* 10^n)
 */

void Show_ENormEIA96(uint32_t Value, int8_t Scale)
{
  uint8_t           n;             /* number of norm values (1 or 2) */

  /*
   * get E series norm values
   * - 1st: Semi.I_value, Semi.I_scale, Semi.A (index number)
   * - 2nd: Semi.C_value, Semi.C_scale, Semi.B (index number)
   */

  n = GetENormValue(Value, Scale, E96, 10);

  /*
   * show E series and tolerance
   */

  Display_NextLine();                      /* move to next line */

  /* display E series (E96) */
  Display_Char('E');                       /* display: E */
  Display_FullValue(96, 0, 0);             /* display E series */

  Display_Space();                         /* display: " " */

  /* display tolerance (1%) */
  Display_FullValue(1, 0, '%');            /* display tolerance */

  Display_Space();                         /* display: " " */


  /*
   * show EIA-96 codes of norm values
   */

  if (n)                             /* got norm value(s) */
  {
    /* display EIA-96 code of first norm value */
    Display_EIA96(Semi.A, Semi.I_scale);

    if (n == 2)              /* got two norm values */
    {
      Display_Space();                         /* display: " " */

      /* display EIA-96 code of second norm value */
      Display_EIA96(Semi.B, Semi.C_scale);
    }
  }
  else                               /* no norm value */
  {
    Display_Minus();           /* display: - */
  }
}

#endif



/*
 * show failed test
 * - no component found
 */

void Show_Fail(void)
{
  /* display info */
  #ifdef UI_CENTER_ALIGN
    Display_CenterLine(2);                       /* center block: 2 lines */
    Display_EEString_Center(Failed1_str);        /* display: No component */
    Display_NL_EEString_Center(Failed2_str);     /* display: found! */
  #else
    Display_EEString(Failed1_str);       /* display: No component */
    Display_NL_EEString(Failed2_str);    /* display: found! */
  #endif

  #ifdef UI_QUESTION_MARK
  /* display question mark symbol */
  Check.Symbol = SYMBOL_QUESTIONMARK;    /* set symbol ID */
  Display_FancySemiPinout(3);            /* show symbol starting in line #3 */
  #endif

  #if CYCLE_MAX < 255
  MissedParts++;           /* increase counter */
  #endif
}



/*
 * show error
 */

void Show_Error()
{
  if (Check.Type == TYPE_DISCHARGE)          /* discharge failed */
  {
    /* possibly a voltage source */
    Display_EEString(DischargeFailed_str);   /* display: Battery? */

    /* display probe number and remaining voltage */
    Display_NextLine();
    Display_ProbeNumber(Check.Probe);
    Display_Colon();
    Display_Space();
    Display_Value(Check.U, -3, 'V');
  }
  else if (Check.Type == TYPE_DETECTION)     /* detection error */
  {
    /* simply display: No component found! */
    Show_Fail();
  }
}



#ifdef UI_PROBE_COLORS

/*
 * show single (first) resistor
 *
 * requires:
 * - Probe1: probe ID #1 [0-2]
 * - Probe2: probe ID #2 [0-2]
 * - Mode: 0 for probe ID
 * >0 for pin designator
 */

void Show_SingleResistor(uint8_t Probe1, uint8_t Probe2, int8_t Mode)
{
  Resistor_Type     *Resistor;     /* pointer to resistor */

  Resistor = &Resistors[0];        /* pointer to first resistor */

  /*
   * show pinout
   */

  if (Mode)
  {
    Display_SemiPinDesignator(Probe1);
  }
  else
  {
    Display_ProbeNumber(Probe1);
  }

  Display_EEString(Resistor_str);

  if (Mode)
  {
    Display_SemiPinDesignator(Probe2); 
  }
  else
  {
    Display_ProbeNumber(Probe2);
  }

  /* show resistance value */
  Display_Space();
  Display_Value(Resistor->Value, Resistor->Scale, LCD_CHAR_OMEGA);
}

#else

/*
 * show single (first) resistor
 *
 * requires:
 * - ID1: pin ID #1 character
 * - ID2: pin ID #2 character
 */

void Show_SingleResistor(uint8_t ID1, uint8_t ID2)
{
  Resistor_Type     *Resistor;     /* pointer to resistor */

  Resistor = &Resistors[0];        /* pointer to first resistor */

  /* show pinout */
  Display_Char(ID1);
  Display_EEString(Resistor_str);
  Display_Char(ID2); 

  /* show resistance value */
  Display_Space();
  Display_Value(Resistor->Value, Resistor->Scale, LCD_CHAR_OMEGA);
}

#endif



/*
 * show resistor(s)
 */

void Show_Resistor(void)
{
  Resistor_Type     *R1;           /* pointer to resistor #1 */
  Resistor_Type     *R2;           /* pointer to resistor #2 */
  uint8_t           Pin;           /* ID of common pin */

  R1 = &Resistors[0];              /* pointer to first resistor */

  if (Check.Resistors == 1)        /* single resistor */
  {
    R2 = NULL;                     /* disable second resistor */
    Pin = R1->A;                   /* make B the first pin */
  }
  else                             /* multiple resistors */
  {
    R2 = R1;
    R2++;                          /* pointer to second resistor */
    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Quantity = 2;             /* got two */
    #endif

    if (Check.Resistors == 3)      /* three resistors */
    {
      Resistor_Type     *Rmax;     /* pointer to largest resistor */   

      /*
       * 3 resistors mean 2 single resistors and both resistors in series.
       * So we have to single out that series resistor by finding the
       * largest resistor.
       */

      Rmax = R1;                   /* starting point */
      for (Pin = 1; Pin <= 2; Pin++)
      {
        if (CmpValue(R2->Value, R2->Scale, Rmax->Value, Rmax->Scale) == 1)
        {
          Rmax = R2;           /* update largest one */
        }

        R2++;                  /* next one */
      }

      /* get the two smaller resistors */
      if (R1 == Rmax) R1++;
      R2 = R1;
      R2++;
      if (R2 == Rmax) R2++;
    }

    /* find common pin of both resistors */
    if ((R1->A == R2->A) || (R1->A == R2->B))
    {
      Pin = R1->A;             /* pin A */
    }
    else
    {
      Pin = R1->B;             /* pin B */
    }
  }

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Comp1 = (void *)R1;         /* link first resistor */
  Info.Comp2 = (void *)R2;         /* link second resistor */
  #endif


  /*
   * display the resistor(s) and pins in line #1
   */

  #ifdef UI_COLORED_TITLES
  Display_UseTitleColor();         /* use title color */
  #endif

  /* first resistor */
  if (R1->A != Pin)                /* A is not common pin */
  {
    Display_ProbeNumber(R1->A);    /* display pin A */
  }
  else                             /* single resistor or A is common pin */
  {
    Display_ProbeNumber(R1->B);    /* display pin B */
  }

  Display_EEString(Resistor_str);
  Display_ProbeNumber(Pin);        /* display common pin */

  if (R2)              /* second resistor */
  {
    Display_EEString(Resistor_str);

    if (R2->A != Pin)          /* A is not common pin */
    {
      Display_ProbeNumber(R2->A);  /* display pin A */
    }
    else                       /* A is common pin */
    {
      Display_ProbeNumber(R2->B);  /* display pin B */
    }
  }

  #ifdef UI_COLORED_TITLES
  Display_UseOldColor();           /* use old color */
  #endif


  /*
   * display the value(s) in line #2
   * - optionally inductance
   * - optionally E-series norm values
   */

  /* first resistor */
  Display_NextLine();
  Display_Value(R1->Value, R1->Scale, LCD_CHAR_OMEGA);

  if (R2)                  /* second resistor */
  {
    Display_Space();
    Display_Value(R2->Value, R2->Scale, LCD_CHAR_OMEGA);

    #ifdef SW_R_TRIMMER
    /* potentiometer/trimpot */
    uint32_t          R_Value;         /* value of R1 */
    uint32_t          Rt_Value;        /* value of R_t */
    int8_t            Scale;           /* scale of R1 and R_t */

    /* normalize R1 and R2 */
    Scale = NormalizeValue(R1->Value, R1->Scale, R2->Value, R2->Scale);
    R_Value = RescaleValue(R1->Value, R1->Scale, Scale);    /* normalized R1 */
    Rt_Value = RescaleValue(R2->Value, R2->Scale, Scale);   /* normalized R2 */

    Rt_Value += R_Value;                           /* R2 + R1 */
    if (Rt_Value > 0)                              /* sanity check */
    {
      /* show sum: R1 + R2 */
      Display_NL_EEString_Space(R_t_str);              /* display: R_t */
      Display_Value(Rt_Value, Scale, LCD_CHAR_OMEGA);  /* display sum */

      /* show R1 ratio (in %): R1 / (R1 + R2) */
      Display_NL_EEString_Space(R1_str);               /* display: R1 */
      R_Value *= 100;                                  /* for % */
      R_Value /= Rt_Value;                             /* R1 / (R1 + R2) */
      Display_Value(R_Value, 0, '%');                  /* display ratio in % */

      /* show R2 ratio (in %): R2 / (R1 + R2) or 100% - ratio_R1 */
      Display_Space();                                 /* display space */
      Display_EEString_Space(R2_str);                  /* display: R2 */
      R_Value = 100 - R_Value;                         /* 100% - ratio_R1 */
      Display_Value(R_Value, 0, '%');                  /* display ratio in % */
    }
    #endif
  }
  #ifdef SW_INDUCTOR
  else                             /* single resistor */
  {
    /* get inductance and display if relevant */
    if (MeasureInductor(R1) == 1)          /* inductor */
    {
      Display_Space();
      Display_Value(Inductor.Value, Inductor.Scale, 'H');

      #ifdef UI_SERIAL_COMMANDS
      /* set data for remote commands */
      Info.Flags |= INFO_R_L;      /* inductance measured */
      #endif

      #ifdef SW_L_E6_T
      /* show E series norm values for E6 20% */
      Show_ENormValues(Inductor.Value, Inductor.Scale, E6, 200, 'H');
      #endif

      #ifdef SW_L_E12_T
      /* show E series norm values for E12 10% */
      Show_ENormValues(Inductor.Value, Inductor.Scale, E12, 100, 'H');
      #endif
    }
    #ifdef SW_R_EXX
    else                               /* no inductance */
    {
      #ifdef SW_R_E24_5_T
      /* show E series norm values for E24 5% */
      Show_ENormValues(R1->Value, R1->Scale, E24, 50, LCD_CHAR_OMEGA);
      #endif

      #ifdef SW_R_E24_5_CC
      /* show E series norm value color-codes for E24 5% */
      Show_ENormCodes(R1->Value, R1->Scale, E24, 50, COLOR_CODE_GOLD);
      #endif

      #ifdef SW_R_E24_1_T
      /* show E series norm values for E24 1% */
      Show_ENormValues(R1->Value, R1->Scale, E24, 10, LCD_CHAR_OMEGA);
      #endif

      #ifdef SW_R_E24_1_CC
      /* show E series norm value color-codes for E24 1% */
      Show_ENormCodes(R1->Value, R1->Scale, E24, 10, COLOR_CODE_BROWN);
      #endif

      #ifdef SW_R_E96_T
      /* show E series norm values for E96 1% */
      Show_ENormValues(R1->Value, R1->Scale, E96, 10, LCD_CHAR_OMEGA);
      #endif

      #ifdef SW_R_E96_CC
      /* show E series norm value color-codes for E96 1% */
      Show_ENormCodes(R1->Value, R1->Scale, E96, 10, COLOR_CODE_BROWN);
      #endif

      #ifdef SW_R_E96_EIA96
      /* show E series norm value EIA-96 codes for E96 1% */
      Show_ENormEIA96(R1->Value, R1->Scale);
      #endif
    }
    #endif
  }
  #elif defined (SW_R_EXX)
  else                                     /* single resistor */
  {
    #ifdef SW_R_E24_5_T
    /* show E series norm values for E24 5% */
    Show_ENormValues(R1->Value, R1->Scale, E24, 50, LCD_CHAR_OMEGA);
    #endif

    #ifdef SW_R_E24_5_CC
    /* show E series norm value color-codes for E24 5% */
    Show_ENormCodes(R1->Value, R1->Scale, E24, 50, COLOR_CODE_GOLD);
    #endif

    #ifdef SW_R_E24_1_T
    /* show E series norm values for E24 1% */
    Show_ENormValues(R1->Value, R1->Scale, E24, 10, LCD_CHAR_OMEGA);
    #endif

    #ifdef SW_R_E24_1_CC
    /* show E series norm value color-codes for E24 1% */
    Show_ENormCodes(R1->Value, R1->Scale, E24, 10, COLOR_CODE_BROWN);
    #endif

    #ifdef SW_R_E96_T
    /* show E series norm values for E96 1% */
    Show_ENormValues(R1->Value, R1->Scale, E96, 10, LCD_CHAR_OMEGA);
    #endif

    #ifdef SW_R_E96_CC
    /* show E series norm value color-codes for E96 1% */
    Show_ENormCodes(R1->Value, R1->Scale, E96, 10, COLOR_CODE_BROWN);
    #endif

    #ifdef SW_R_E96_EIA96
    /* show E series norm value EIA-96 codes for E96 1% */
    Show_ENormEIA96(R1->Value, R1->Scale);
    #endif
  }
  #endif
}


/*
 * show capacitor
 */

void Show_Capacitor(void)
{
  Capacitor_Type    *MaxCap;         /* pointer to largest cap */
  Capacitor_Type    *Cap;            /* pointer to cap */
  #if defined (SW_ESR) || defined (SW_OLD_ESR)
  uint16_t          ESR;             /* ESR (in 0.01 Ohms) */
  #endif
  uint8_t           Counter;         /* loop counter */

  /*
   * find largest cap
   */

  MaxCap = &Caps[0];                 /* pointer to first cap */
  Cap = MaxCap;

  for (Counter = 1; Counter <= 2; Counter++)
  {
    Cap++;                             /* next cap */

    if (CmpValue(Cap->Value, Cap->Scale, MaxCap->Value, MaxCap->Scale) == 1)
    {
      MaxCap = Cap;
    }
  }

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Comp1 = (void *)MaxCap;       /* link largest cap */
  #endif


  /*
   * display cap and pinout in line #1
   */

  #ifdef UI_COLORED_TITLES
  Display_UseTitleColor();           /* use title color */
  #endif

  Display_ProbeNumber(MaxCap->A);    /* display pin #1 */
  Display_EEString(Cap_str);         /* display capacitor symbol */
  Display_ProbeNumber(MaxCap->B);    /* display pin #2 */

  #ifdef UI_COLORED_TITLES
  Display_UseOldColor();             /* use old color */
  #endif


  /*
   * display capacitance in line #2, optionally ESR
   */

  /* display capacitance */
  Display_NextLine();                /* move to next line */
  Display_Value(MaxCap->Value, MaxCap->Scale, 'F');

  #if defined (SW_ESR) || defined (SW_OLD_ESR)
  /* measure and display ESR */
  ESR = MeasureESR(MaxCap);          /* measure ESR */
  if (ESR < UINT16_MAX)              /* if successful */
  {
    Display_Space();
    Display_Value(ESR, -2, LCD_CHAR_OMEGA);  /* display ESR */
  }
    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Val1 = ESR;                 /* copy ESR */
    #endif
  #endif


  /*
   * display additional stuff
   */

  /* display self-discharge equivalent leakage current */
  if (MaxCap->I_leak_Value > 0)          /* got value */
  {
    Display_NL_EEString_Space(I_leak_str);
    Display_Value(MaxCap->I_leak_Value, MaxCap->I_leak_Scale, 'A');  /* in A */
  }

  #ifdef SW_C_VLOSS
  /* display self-discharge voltage loss in % */
  if (MaxCap->U_loss > 0)                /* got value */
  {
    Display_NL_EEString_Space(U_loss_str);
    Display_Value(MaxCap->U_loss, -1, '%');  /* in 0.1% */
  }
  #endif

  #ifdef SW_C_E6_T
  /* show E series norm values for E6 20% */
  Show_ENormValues(MaxCap->Value, MaxCap->Scale, E6, 200, 'F');
  #endif

  #ifdef SW_C_E12_T
  /* show E series norm values for E12 10% */
  Show_ENormValues(MaxCap->Value, MaxCap->Scale, E12, 100, 'F');
  #endif
}



/*
 * show current (leakage or whatever) of semiconductor
 *
 * Mapping for Semi structure:
 * - I_value - current 
 * - I_scale - scale for current (10^x)
 */

void Show_SemiCurrent(const unsigned char *String)
{
  if (CmpValue(Semi.I_value, Semi.I_scale, 50, -9) >= 0)  /* show if >=50nA */
  {
    Display_NL_EEString_Space(String);               /* display: <string> */
    Display_Value(Semi.I_value, Semi.I_scale, 'A');  /* display current */
  }
}



#ifndef UI_SERIAL_COMMANDS

/*
 * display capacitance of a diode
 *
 * requires:
 * - pointer to diode structure
 */

void Show_Diode_Cap(Diode_Type *Diode)
{
  /* get capacitance (reversed direction) */
  MeasureCap(Diode->C, Diode->A, 0);

  /* and show capacitance */
  Display_Value(Caps[0].Value, Caps[0].Scale, 'F');
}

#endif



/*
 * show flyback diode of 3-pin semiconductor and pin designators
 * - convenience function to reduce firmware size
 *
 * requires:
 * - Anode:    probe ID of anode [0-2]
 * - Cathode: probe ID of cathode [0-2]
 */

void Show_SemiFlybackDiode(uint8_t Anode, uint8_t Cathode)
{
  Display_SemiPinDesignator(Anode);      /* designator for anode side */
  Display_Char(LCD_CHAR_DIODE_AC);       /* diode symbol |>| */
  Display_SemiPinDesignator(Cathode);    /* designator for cathode side */
}



/*
 * show diode(s)
 */

void Show_Diode(void)
{
  Diode_Type        *D1;             /* pointer to diode #1 */
  Diode_Type        *D2 = NULL;      /* pointer to diode #2 */
  uint8_t           CapFlag = 1;     /* flag for capacitance output */
  uint8_t           A = 5;           /* ID of common anode */
  uint8_t           C = 5;           /* ID of common cathode */
  uint8_t           R_Pin1 = 5;      /* B_E resistor's pin #1 */
  uint8_t           R_Pin2 = 5;      /* B_E resistor's pin #2 */
  uint8_t           n;               /* counter */
  uint8_t           m;               /* counter */

  D1 = &Diodes[0];                   /* pointer to first diode */

  #ifdef UI_COLORED_TITLES
  Display_UseTitleColor();           /* use title color */
  #endif

  /*
   * figure out which diodes to display
   */

  if (Check.Diodes == 1)             /* single diode */
  {
    C = D1->C;                       /* make anode first pin */
  }
  else if (Check.Diodes == 2)        /* two diodes */
  {
    D2 = D1;                         /* copy pointer to first diode */
    D2++;                            /* pointer to second diode */

    if (D1->A == D2->A)              /* common anode */
    {
      A = D1->A;                     /* save common anode */

      /* possible PNP BJT with low value B-E resistor and flyback diode */
      R_Pin1 = D1->C;
      R_Pin2 = D2->C;
    }
    else if (D1->C == D2->C)         /* common cathode */
    {
      C = D1->C;                     /* save common cathode */

      /* possible NPN BJT with low value B-E resistor and flyback diode */
      R_Pin1 = D1->A;
      R_Pin2 = D2->A;
    }
    else if ((D1->A == D2->C) && (D1->C == D2->A))   /* anti-parallel */
    {
      A = D1->A;                     /* anode and cathode */
      C = A;                         /* are the same */
      CapFlag = 0;                   /* skip capacitance */
    }
  }
  else if (Check.Diodes == 3)        /* three diodes */
  {
    /*
     * Two diodes in series are detected as a virtual third diode:
     * - Check for any possible way the 2 diodes could be connected in series.
     * - Only once the cathode of diode #1 matches the anode of diode #2.
     */

    for (n = 0; n <= 2; n++)         /* loop for first diode */
    {
      D1 = &Diodes[n];               /* get pointer of first diode */

      for (m = 0; m <= 2; m++)       /* loop for second diode */
      {
        D2 = &Diodes[m];             /* get pointer of second diode */

        if (n != m)                  /* don't check same diode :-) */
        {
          if (D1->C == D2->A)        /* got match */
          {
            n = 5;                   /* end loops */
            m = 5;
          }
        }
      }
    }

    if (n < 5) D2 = NULL;          /* no match found */
    C = D1->C;                       /* cathode of first diode */
    A = 3;                           /* in series mode */
  }
  else                               /* too many diodes */
  {
    /* display number of diodes found in line #1 */
    Display_EEString_Space(Diode_AC_str);    /* display: -|>|- */
    Display_Char('0' + Check.Diodes);        /* display number of diodes */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Quantity = Check.Diodes;            /* set quantity */
    #endif

    return;
  }

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Comp1 = (void *)D1;         /* link first diode */
  Info.Comp2 = (void *)D2;         /* link second diode */
  #endif


  /*
   * display diode(s) and pinout in line #1
   */

  /* first diode */
  if (A < 3)           /* common anode or anti-parallel: show C first */
  {
    Display_ProbeNumber(D1->C);          /* show C */
    Display_EEString(Diode_CA_str);      /* show -|<- */
    Display_ProbeNumber(A);              /* show A */
  }
  else                 /* single, common cathode or in-series: show A first */
  {
    Display_ProbeNumber(D1->A);          /* show A */
    Display_EEString(Diode_AC_str);      /* show ->|- */
    Display_ProbeNumber(C);              /* show C */
  }

  if (D2)              /* second diode */
  {
    if (A == C)            /* anti-parallel */
    {
      n = D2->A;                         /* get anode */
      Display_EEString(Diode_CA_str);    /* show -|<- */
    }
    else if (A <= 3)       /* common anode or in-series */
    {
      n = D2->C;                         /* get cathode */
      Display_EEString(Diode_AC_str);    /* show ->|- */
    }
    else                   /* common cathode */
    {
      n = D2->A;                         /* get anode */
      Display_EEString(Diode_CA_str);    /* show -|<- */
    }

    Display_ProbeNumber(n);              /* display pin */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Quantity = 2;         /* got two */
    #endif
  }

  #ifdef UI_COLORED_TITLES
  Display_UseOldColor();           /* use old color */
  #endif


  /*
   * check for B-E resistor of possible BJT
   */

  if (R_Pin1 < 5)                    /* possible BJT */
  {
    /* B-E resistor below 25kOhms */
    if (CheckSingleResistor(R_Pin1, R_Pin2, 25) == 1)
    {
      /* show: PNP/NPN? */
      Display_Space();
      if (A < 3)                         /* PNP */
      {
        Display_EEString(PNP_str);       /* display: PNP */
        #ifdef UI_SERIAL_COMMANDS
        /* set data for remote commands */
        Info.Flags |= INFO_D_R_BE | INFO_D_BJT_PNP;     /* R_BE & PNP */
        #endif
      }
      else                               /* NPN */
      {
        Display_EEString(NPN_str);       /* display: NPN */
        #ifdef UI_SERIAL_COMMANDS
        /* set data for remote commands */
        Info.Flags |= INFO_D_R_BE | INFO_D_BJT_NPN;     /* R_BE & NPN */
        #endif
      }
      Display_Char('?');                 /* display: ? */

      Display_NextLine();                /* move to line #2 */

      /* show B-E resistor */
      #ifdef UI_PROBE_COLORS
        Show_SingleResistor(R_Pin1, R_Pin2, 0);    /* show resistor */
      #else 
        R_Pin1 += '1';                   /* convert probe ID to character */
        R_Pin2 += '1';
        Show_SingleResistor(R_Pin1, R_Pin2);      /* show resistor */
      #endif

      CapFlag = 0;                       /* skip capacitance */
    }
  }


  /*
   * display:
   * - Uf (forward voltage)
   * - reverse leakage current (for single diode)
   * - capacitance (not for anti-parallel diodes)
   */

  /* display Uf */
  Display_NL_EEString_Space(Vf_str);    /* display: Vf */

  /* first diode */
  Display_Value(D1->V_f, -3, 'V');      /* in mV */

  Display_Space();

  /* display low current Uf and reverse leakage current for a single diode */
  if (D2 == NULL)                        /* single diode */
  {
    /* display low current Uf if it's quite low (Ge/Schottky diode) */
    if (D1->V_f2 < 250)              /* < 250mV */
    {
      Display_Char('(');
      Display_Value(D1->V_f2, 0, 0);    /* no unit */
      Display_Char(')');
    }

    /* reverse leakage current */
    UpdateProbes2(D1->C, D1->A);         /* reverse diode */
    GetLeakageCurrent(1);                /* get current */
    Show_SemiCurrent(I_R_str);           /* display I_R */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_D_I_R;            /* measured I_R */
    #endif
  }
  else                                   /* two diodes */
  {
    /* show Uf of second diode */
    Display_Value(D2->V_f, -3, 'V');
  }


  /*
   * display capacitance in next line
   */

  if (CapFlag == 1)                      /* if feasable */ 
  {
    Display_NL_EEString_Space(DiodeCa
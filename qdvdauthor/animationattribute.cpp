/***********************************************************************
 * \file animationattribute.cpp
 * 
 * This class will handle the Animation attributes.
 * It creates an array for every USED attribute 
 * which is to be modified during the animation.
 * 
 * If the Attribute is a simple type (int, float, double etc)
 * then the 
 *
 *   Created :
 *        by : Varol Okan using Kate
 * Copyright : (c) Varol Okan
 *   License : GPL v 2.0
 *
 ***********************************************************************/

#include "animationattribute.h"

/**
 * Constructor for integers with a function to modify the actual value.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, int iOrigValue, CallbackClass *fnc )
{
	long t;
	initMe (iFrames, qsName);
	m_setFunction  = fnc; 
	m_arrayInts    = new int[iFrames+1];
	for (t=0;t<iFrames+1;t++)
		m_arrayInts[t] = -1;
	m_arrayInts[0] = iOrigValue;
}

/**
 * Constructor for strings which are modified through a function.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, QString qsOrigValue, CallbackClass *fnc )
{
	initMe (iFrames, qsName);
	m_setFunction  = fnc; 
	m_arrayStrings    = new QString[iFrames+1];
	m_arrayStrings[0] = qsOrigValue;
}

/**
 * Constructor for integers where we have direct access to the attributes address location.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, int iOrigValue, int *pTheLocation)
{
	long t;
	initMe (iFrames, qsName);
	m_arrayInts    = new int[iFrames+1];
	for (t=0;t<iFrames+1;t++)
		m_arrayInts[t] = -1;
	m_piValue      = pTheLocation;
	m_arrayInts[0] = iOrigValue;
}

/**
 * Constructor for booleans.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, bool bOrigValue, bool *pTheLocation)
{
	long t;
	initMe (iFrames, qsName);
	m_arrayBools    = new bool[iFrames+1];
	for (t=0;t<iFrames+1;t++)
		m_arrayBools[t] = false;
	m_pbValue       = pTheLocation;
	m_arrayBools[0] = bOrigValue;
}

/**
 * Constructor for floats.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, float fOrigValue, float *pTheLocation)
{
	long t;
	initMe (iFrames, qsName);
	m_arrayFloats    = new float[iFrames+1];
	for (t=0;t<iFrames+1;t++)
		m_arrayFloats[t] = -9999.99f;
	m_pfValue        = pTheLocation;
	m_arrayFloats[0] = fOrigValue;
}

/**
 * Constructor for doubles.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, double fOrigValue, double *pTheLocation)
{
	long t;
	initMe (iFrames, qsName);
	m_arrayDoubles    = new double[iFrames+1];
	for (t=0;t<iFrames+1;t++)
		m_arrayDoubles[t] = -9999.99f;
	m_pdValue         = pTheLocation;
	m_arrayDoubles[0] = fOrigValue;
}

/**
 * Constructor for Strings where we have direct access to thr memory location of the string.
 */
AnimationAttribute::AnimationAttribute (long iFrames, QString qsName, QString qsOrigValue, QString *pTheLocation)
{
	initMe (iFrames, qsName);
	m_arrayStrings    = new QString[iFrames+1];
	m_pqsValue        = pTheLocation;
	m_arrayStrings[0] = qsOrigValue;
}

/**
 * default destructor. free all allocated memory.
 */
AnimationAttribute::~AnimationAttribute ()
{
	if (m_arrayInts)
		delete []m_arrayInts;
	if (m_arrayBools)
		delete []m_arrayBools;
	if (m_arrayStrings)
		delete []m_arrayStrings;
	if (m_arrayFloats)
		delete []m_arrayFloats;
	if (m_arrayDoubles)
		delete []m_arrayDoubles;
	if (m_setFunction)
		delete m_setFunction;
	m_arrayInts    = NULL;
	m_arrayFloats  = NULL;
	m_arrayStrings = NULL;
	m_setFunction  = NULL;
}

void AnimationAttribute::initMe (long iFrames, QString qsName)
{
	m_setFunction  = NULL; 
	m_piValue      = NULL;
	m_pbValue      = NULL;
	m_pfValue      = NULL;
	m_pqsValue     = NULL;
	m_arrayBools   = NULL;
	m_arrayFloats  = NULL;
	m_arrayDoubles = NULL;
	m_arrayStrings = NULL;
	m_arrayInts    = NULL;
	m_qsName       = qsName;
	m_iMaxFrames   = iFrames;
	m_bNeedRefresh = true;
}

/**
 * This function returns wether we will need to refresh the imageObject or not.
 */
bool AnimationAttribute::needRefresh ( )
{
  return m_bNeedRefresh;
}

void AnimationAttribute::setNeedRefresh ( bool bRefresh )
{
  m_bNeedRefresh = bRefresh;
}

/**
 * This function will return the name of the attribute.
 */
const QString AnimationAttribute::name ()
{
	return m_qsName;
}

/**
 * This function will return the int at array [iFrame]
 */
int AnimationAttribute::iValue (long iFrame)
{
	if (!m_arrayInts)
		return -1;
	if ( ( iFrame < 0 ) || ( iFrame > m_iMaxFrames ) )
		return -1;
	return (m_arrayInts[iFrame]);
}

/**
 * This function will return the bool at array [iFrame]
 */
bool AnimationAttribute::bValue (long iFrame)
{
	if (!m_arrayBools)
		return false;
	if ( ( iFrame < 0 ) || ( iFrame > m_iMaxFrames ) )
		return false;
	return (m_arrayBools[iFrame]);
}

/**
 * This function will return the float at array [iFrame]
 */
float AnimationAttribute::fValue (long iFrame)
{
	if (!m_arrayFloats)
		return 0.0;
	if ( ( iFrame < 0 ) || ( iFrame > m_iMaxFrames ) )
		return 0.0;
	return (m_arrayFloats[iFrame]);
}

/**
 * This function will return the double at array [iFrame]
 */
double AnimationAttribute::dValue (long iFrame)
{
	if (!m_arrayDoubles)
		return 0.0;
	if ( ( iFrame < 0 ) || ( iFrame > m_iMaxFrames ) )
		return 0.0;
	return (m_arrayDoubles[iFrame]);
}

/**
 * This function will return the QString at array [iFrame]
 */
QString AnimationAttribute::qsValue (long iFrame)
{
	if (!m_arrayStrings)
		return QString ();
	if ( ( iFrame < 0 ) || ( iFrame > m_iMaxFrames ) )
		return QString ();
	return (m_arrayStrings[iFrame]);
}

/**
 * Returns the max number of frames the internal array can hold.
 */
long AnimationAttribute::maxFrames ()
{
	return m_iMaxFrames;;
}

/** 
 * This is the main function which is called when the animation is ongoing.
 * It will retrieve the stored value from the array and either modify the value
 * directly or it will call a function to do the modification.
 * 
 * @param iFrame The frame number for which we are getting the value from the array.
 */
bool AnimationAttribute::setValue (long iFrame)
{
	if ( ( ! m_arrayFloats ) && ( ! m_arrayInts ) &&
	     ( ! m_arrayBools )  && ( ! m_arrayDoubles ) )
		return false;
	if ( ( iFrame < 0 ) || ( iFrame > m_iMaxFrames ) )
		return false;

	// Do we have an integer array ???
	if (m_arrayInts && m_piValue)	{
		if (m_arrayInts[iFrame] == -1)
			return false;
		*m_piValue = m_arrayInts[iFrame];
	} // or is it a
	else if (m_arrayInts && m_setFunction)	{
		if (m_arrayInts[iFrame] == -1)
			return false;
		(*m_setFunction)(m_arrayInts[iFrame]);
	}

	else if (m_arrayFloats && m_pfValue)	{
		if (m_arrayFloats[iFrame] == -9999.99f)
			return false;
		*m_pfValue = m_arrayFloats[iFrame];
	}
	else if (m_arrayDoubles && m_pdValue)	{
		if (m_arrayDoubles[iFrame] == -9999.99f)
			return false;
		*m_pdValue = m_arrayDoubles[iFrame];
	}
	else if (m_arrayStrings && m_pqsValue)	{
		if (m_arrayStrings[iFrame].isEmpty())
			return false;
		*m_pqsValue = m_arrayStrings[iFrame];
	}
	return m_bNeedRefresh;
}

/** 
 * This function will set the passed value on the right spot in the array.
 * The input is always a QString, which will be converted into int, float,
 * double, or be kept as a QString.
 */
void AnimationAttribute::append (long iFrame, QString qsValue)
{
	if ( (iFrame < 0) || (iFrame > m_iMaxFrames) )
		return;
	if (m_arrayInts)
		m_arrayInts[iFrame] = qsValue.toInt ();
	else if (m_arrayFloats)
		m_arrayFloats[iFrame] = qsValue.toFloat ();
	else if (m_arrayDoubles)
		m_arrayDoubles[iFrame] = qsValue.toDouble ();
	else
		m_arrayStrings[iFrame] = qsValue;
}


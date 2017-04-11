/**
 * Licensed under Creative Commons Attribution 3.0 Unported license.
 * http://creativecommons.org/licenses/by/3.0/
 * You are free to copy, distribute and transmit the work, and 
 * to adapt the work.  You must attribute android-plist-parser 
 * to Free Beachler (http://www.freebeachler.com).
 * 
 * The Android PList parser (android-plist-parser) is distributed in 
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without 
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE.
 */
package android.xml.plist;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.ext.DefaultHandler2;

import android.util.Log;

import android.xml.plist.domain.PList;
import android.xml.plist.domain.PListObject;

/**
 * <p>
 * Parse the a PList. Documentation on PLists can be found at: <a href=
 * "http://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/PropertyLists/AboutPropertyLists/AboutPropertyLists.html#//apple_ref/doc/uid/10000048i-CH3-SW2"
 * >The Mac OS X Reference</a>
 * </p>
 * 
 * 
 * @author fbeachler
 * 
 */
public class PListXMLHandler extends DefaultHandler2 {

	public static final java.lang.String TAG = "PListXMLHandler";

	/**
	 * Defines the modes the parser reports to registered listeners.
	 * 
	 * @author fbeachler
	 * 
	 */
	public enum ParseMode {
		START_TAG, END_TAG
	};

	/**
	 * Implementors can listen for events defined by {@link ParseMode}.
	 * 
	 * @author fbeachler
	 * 
	 */
	public static interface PListParserListener {
		public void onPListParseDone(PList pList, ParseMode mode);
	}

	/**
	 * {@link PListXMLStringer} for this class.
	 */
	private PListXMLStringer PListXMLStringer;

	/**
	 * Listener for this parser.
	 */
	private PListParserListener parseListener;

	/**
	 * The value of parsed characters from elements and attributes.
	 */
	private PListXMLStringer tempVal;

	/**
	 * The parsed {@link PList}.
	 */
	private PList pList;

	// Registers to hold state of parsing the workflow as Dict
	protected java.lang.String key;

	/**
	 * 
	 */
	public PListXMLHandler() {
		super();
		PListXMLStringer = new PListXMLStringer();
	}

	/**
	 * @return the pList
	 */
	public PList getPlist() {
		return pList;
	}

	/**
	 * @param pList
	 *            the pList to set
	 */
	public void setPlist(PList plist) {
		this.pList = plist;
	}

	/**
	 * @return the parseListener
	 */
	public PListParserListener getParseListener() {
		return parseListener;
	}

	/**
	 * @param parseListener
	 *            the parseListener to set
	 */
	public void setParseListener(PListParserListener parseListener) {
		this.parseListener = parseListener;
	}

	/**
	 * @return the tempVal
	 */
	public PListXMLStringer getTempVal() {
		return tempVal;
	}

	/**
	 * @param tempVal
	 *            the tempVal to set
	 */
	public void setTempVal(PListXMLStringer tempVal) {
		this.tempVal = tempVal;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#startDocument()
	 */
	@Override
	public void startDocument() throws SAXException {
		super.startDocument();
		tempVal = new PListXMLStringer();
		pList = null;
		key = null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#startElement(java.lang.String,
	 * java.lang.String, java.lang.String, org.xml.sax.Attributes)
	 */
	@Override
	public void startElement(java.lang.String uri, java.lang.String localName,
			java.lang.String qName, Attributes attributes) throws SAXException {
		Log.v(PListXMLStringer.newBuilder().append(TAG).append("#startElement")
				.toString(),
				PListXMLStringer.newBuilder()
						.append("Start Element lname|uri|attr.length :")
						.append(localName).append(PListXMLConstants.PIPE).append(uri)
						.append(PListXMLConstants.PIPE).append(attributes.getLength())
						.toString());
		tempVal.newBuilder();
		if (localName.equalsIgnoreCase(PListXMLConstants.TAG_PLIST)) {
			if (null != pList) {
				// there should only be one PList element in the root
				throw new SAXException(
						"there should only be one PList element in PList XML");
			}
			pList = new PList();
		} else {
			if (null == pList) {
				throw new SAXException(
						"invalid PList - please see http://www.apple.com/DTDs/PropertyList-1.0.dtd");
			}
			if (localName.equalsIgnoreCase(PListXMLConstants.TAG_DICT) || 
					localName.equalsIgnoreCase(PListXMLConstants.TAG_PLIST_ARRAY)) {
				try {
					PListObject objToAdd = pList.buildObject(localName, tempVal
							.getBuilder().toString());
					pList.stackObject(objToAdd, key);
				} catch (Exception e) {
					throw new SAXException(e);
				}
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#characters(char[], int, int)
	 */
	@Override
	public void characters(char[] ch, int start, int length)
			throws SAXException {
		Log.v(PListXMLStringer.newBuilder().append(TAG).append("#characters")
				.toString(),
				PListXMLStringer.newBuilder().append(ch).append(PListXMLConstants.PIPE)
						.append(start).append(PListXMLConstants.PIPE).append(length)
						.append(PListXMLConstants.PIPE).toString());
		tempVal.getBuilder().append(new java.lang.String(ch, start, length));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String,
	 * java.lang.String, java.lang.String)
	 */
	@Override
	public void endElement(java.lang.String uri, java.lang.String localName,
			java.lang.String qName) throws SAXException {
		Log.v(PListXMLStringer.newBuilder().append(TAG).append("#endElement")
				.toString(),
				PListXMLStringer.newBuilder().append("localName|qName|uri|tempVal: ")
						.append(localName).append(PListXMLConstants.PIPE).append(qName)
						.append(PListXMLConstants.PIPE).append(uri)
						.append(PListXMLConstants.PIPE)
						.append(tempVal.getBuilder().toString()).toString());
		if (localName.equalsIgnoreCase(PListXMLConstants.TAG_KEY)) {
			key = tempVal.getBuilder().toString().trim();
		} else if (localName.equalsIgnoreCase(PListXMLConstants.TAG_DICT) || 
				localName.equalsIgnoreCase(PListXMLConstants.TAG_PLIST_ARRAY)) {
			pList.popStack();
		} else if (!localName.equalsIgnoreCase(PListXMLConstants.TAG_PLIST)) {
			try {
				PListObject objToAdd = pList.buildObject(localName, tempVal
						.getBuilder().toString());
				pList.stackObject(objToAdd, key);
			} catch (Exception e) {
				throw new SAXException(e);
			}
			key = null;
		} else if (localName.equalsIgnoreCase(PListXMLConstants.TAG_PLIST)) {
			if (null != parseListener) {
				parseListener.onPListParseDone(pList, ParseMode.END_TAG);
			}
		}
		tempVal.newBuilder();

	}

}

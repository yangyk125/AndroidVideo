/**
 * NOTE: portions of this code (PListXMLBase64 encoding/decoding) use code
 * licensed under the BSD agreement.
 * 
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
package android.xml.plist.domain;

import android.xml.plist.PListXMLBase64;
import android.xml.plist.PListXMLStringer;

/**
 * Represents a simple PList data element. The value is stored as a raw string.
 */
public class Data extends PListObject implements
		IPListSimpleObject<java.lang.String> {

	protected PListXMLStringer dataStringer;
	protected byte[] rawData;

	/**
	 * 
	 */
	private static final long serialVersionUID = -3101592260075687323L;

	public Data() {
		setType(PListObjectType.DATA);
		dataStringer = new PListXMLStringer();
	}

	/**
	 * Get the raw PListXMLBase64 encoded data value on this object. Assumes the data
	 * has already been encoded.
	 * 
	 * @see com.longevitysoft.android.xml.plist.PListXMLHandler.PList.
	 *      IPListSimpleObject#getValue()
	 */
	@Override
	public java.lang.String getValue() {
		return getValue(true);
	}

	/**
	 * Get the raw PListXMLBase64 encoded data value on this object. Assumes the data
	 * has already been encoded.
	 * 
	 * @param decode
	 *            - if true, the result is PListXMLBase64 decoded before being returned.
	 *            NOTE: this calls {@link PListXMLBase64#decodeFast(byte[])} which
	 *            expects the raw encoded data to be on one line (no line
	 *            separators). To change this behavior, subclass and override.
	 * 
	 * @see com.longevitysoft.android.xml.plist.PListXMLHandler.PList.
	 *      IPListSimpleObject#getValue()
	 */
	public java.lang.String getValue(boolean decode) {
		dataStringer.newBuilder();
		if (decode) {
			return dataStringer.getBuilder()
					.append(new java.lang.String(PListXMLBase64.decodeFast(rawData)))
					.toString();
		} else {
			return dataStringer.getBuilder().append(rawData).toString();
		}
	}

	/**
	 * Sets the raw PListXMLBase64 data value on this object. Assumes the data is
	 * properly encoded.
	 * 
	 * @param val
	 *            - PListXMLBase64 encoded data to set
	 * 
	 * @see com.longevitysoft.android.xml.plist.PListXMLHandler.PList.
	 *      IPListSimpleObject#setValue(java.lang.Object)
	 */
	@Override
	public void setValue(java.lang.String val) {
		setValue(val, true);
	}

	/**
	 * Sets the data value on this object.
	 * 
	 * @param val
	 *            - unencoded data to set
	 * @param encoded
	 *            - flag true if val is PListXMLBase64 encoded already. If false, val is
	 *            encoded (without line seperators) before being stored.
	 * 
	 * @see com.longevitysoft.android.xml.plist.PListXMLHandler.PList.
	 *      IPListSimpleObject#setValue(java.lang.Object)
	 */
	public void setValue(java.lang.String val, boolean encoded) {
		if (!encoded) {
			rawData = PListXMLBase64.encodeToByte(val.getBytes(), false);
		} else {
			rawData = val.getBytes();
		}
	}

	/**
	 * Sets the data value on this object.
	 * 
	 * @param val
	 *            - unencoded data to set
	 * @param encoded
	 *            - flag true if val is PListXMLBase64 encoded already. If false, val is
	 *            encoded (without line seperators) before being stored.
	 * 
	 * @see com.longevitysoft.android.xml.plist.PListXMLHandler.PList.
	 *      IPListSimpleObject#setValue(java.lang.Object)
	 */
	public void setValue(byte[] val, boolean encoded) {
		if (!encoded) {
			rawData = PListXMLBase64.encodeToByte(val, false);
		} else {
			rawData = val;
		}
	}

}
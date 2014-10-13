package fr.eurecom.senml.entity;

import java.sql.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.HashMap;
import java.util.UUID;
import java.util.logging.Logger;
import java.util.Map;

import javax.jdo.annotations.NotPersistent;
import javax.jdo.annotations.PersistenceCapable;
import javax.jdo.annotations.Persistent;
import javax.jdo.annotations.PrimaryKey;

import org.json.JSONObject;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;

import fr.eurecom.senml.persistence.JDOStorage;

/**
 * Immutable object. Represents the registered device.
 */

@PersistenceCapable(detachable="true")
public class RegisteredDeviceType {

	public enum ParamTypeValue {
		STR("sv");

		private String encoded;
		private final static Map<String, ParamTypeValue> mTypeValue = new HashMap<String, ParamTypeValue>(); 

		ParamTypeValue(String encoded) {
			this.encoded = encoded;
		}

		public String getSenML() {
			return encoded;
		}
		
		public static ParamTypeValue getParamTypeValue(String senmlEncoded) {
			if (mTypeValue == null) {
				initMapValue();
			}
			return mTypeValue.get(senmlEncoded);
		}
		
		private static synchronized void initMapValue() {
			if (mTypeValue.size() > 0) {
				return;
			}
			for (ParamTypeValue v : values()) {
				mTypeValue.put(v.encoded, v);
			}
		}
	};
	
	@NotPersistent
	private static final DateFormat df = new SimpleDateFormat("yyyy.MM.dd G 'at' HH:mm:ss z");

	@NotPersistent
	private static final Logger log = Logger
			.getLogger("sensors.entity.RegisteredDeviceType");

	@NotPersistent
	public static final String PARAM_STRING_VALUE = "sv";
	
	@NotPersistent
	public static final String PARAM_NAME = "n";
	
	@PrimaryKey
	@Persistent
	Key key;

	@Persistent
	private String deviceType;
	
	@Persistent
	private String registrationKey;
	
	@Persistent
	private long time;
	
	private boolean isadmin = false;
	
	public String getDeviceType() {
		return deviceType;
	}
	
	public String getValue() {
		return String.valueOf(registrationKey);
	}
	
	public String getTime() {
		return formatDate(time);
	}

	public RegisteredDeviceType(String deviceKey, String deviceType, 
			String val, long time) {
		this.deviceType = 
	deviceType == null || deviceType.isEmpty() ? generateName() : deviceType;
		this.registrationKey = val;
		this.time = time;
		this.key = KeyFactory.createKey(
				RegisteredDeviceType.class.getSimpleName(),
							deviceKey + deviceType +"" + time);
	}
	
	public void destroy() {
		JDOStorage.getInstance().delete(this);
	}
	
	public String getKey() {
		return key == null ? null : KeyFactory.keyToString(key);
	}
	
	public String getDeviceRegistrationKey() {
		return this.registrationKey;
	}

	public JSONObject toJSONSenML() {
		try {
			JSONObject entrySenML = new JSONObject();
			if (deviceType != null) {
				entrySenML.put(PARAM_NAME, deviceType);
			}
			entrySenML.put(PARAM_STRING_VALUE, registrationKey);
			return entrySenML;
		} catch (Exception e) {
			e.printStackTrace();
			log.warning("Error Encoding a measure");
			return null;
		}
	}
	
	public Node toXMLSenML(Document d) {
		Element measure = d.createElement("e");
		measure.setAttribute(PARAM_NAME, deviceType);
		measure.setAttribute(PARAM_STRING_VALUE, String.valueOf(registrationKey));
		return measure;
	}
	
	public String toString() {
	StringBuilder builder = new StringBuilder();
	builder.append("key[" + getKey())
	.append("] name[" + deviceType)
	.append("] value[" + registrationKey)
	.append("] time[" + formatDate(time))
	.append("]");
	return builder.toString();
	}
	
	public String toPrettyString() {
		StringBuilder builder = new StringBuilder();
		builder.append(deviceType)
		.append(": ")
		.append(") ")
		.append(registrationKey)
		.append(" @ ")
		.append(formatDate(time));
		return builder.toString();
	}
	
	private String formatDate(long ms) {
	if (ms == 0) return "now";
	if (ms < 0) return ("now - " + ms);
	
	return df.format(new Date(ms));
	}
	
	private String generateName() {
		return UUID.randomUUID().toString();
	}

	public boolean isIsadmin() {
		return isadmin;
	}

	public void setIsadmin(boolean isadmin) {
		this.isadmin = isadmin;
	}
}

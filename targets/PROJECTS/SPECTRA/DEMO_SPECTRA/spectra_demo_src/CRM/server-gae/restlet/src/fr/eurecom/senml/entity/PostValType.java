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
 * Immutable object. Represents the SenML Measurement entry.
 * 
 * 
 */

@PersistenceCapable(detachable="true")
public class PostValType {

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
			.getLogger("sensors.entity.PostValType");

	@NotPersistent
	public static final String PARAM_STRING_VALUE = "sv";
	
	@NotPersistent
	public static final String PARAM_NAME = "n";
	
	@PrimaryKey
	@Persistent
	Key key;

	@Persistent
	private String name;
	
	@Persistent
	private String value;
	
	@Persistent
	private long time;
	
	public String getName() {
		return name;
	}
	
	public String getValue() {
		return String.valueOf(value);
	}
	
	public String getTime() {
		return formatDate(time);
	}

	public PostValType(String sensorKey, String name, 
			String val, long time) {
		this.name = name == null || name.isEmpty() ? generateName() : name;
		this.value = val;
		this.time = time;
		this.key = KeyFactory.createKey(PostValType.class.getSimpleName(), sensorKey + name +"" + time);
	}
	
	public void destroy() {
		JDOStorage.getInstance().delete(this);
	}
	
	public String getKey() {
		return key == null ? null : KeyFactory.keyToString(key);
	}

	public JSONObject toJSONSenML() {
		try {
			JSONObject entrySenML = new JSONObject();
			if (name != null) {
				entrySenML.put(PARAM_NAME, name);
			}
			entrySenML.put(PARAM_STRING_VALUE, value);
			return entrySenML;
		} catch (Exception e) {
			e.printStackTrace();
			log.warning("Error Encoding a measure");
			return null;
		}
	}
	
	public Node toXMLSenML(Document d) {
		Element measure = d.createElement("e");
		measure.setAttribute(PARAM_NAME, name);
		measure.setAttribute(PARAM_STRING_VALUE, String.valueOf(value));
		return measure;
	}
	
	public String toString() {
	StringBuilder builder = new StringBuilder();
	builder.append("key[" + getKey())
	.append("] name[" + name)
	.append("] value[" + value)
	.append("] time[" + formatDate(time))
	.append("]");
	return builder.toString();
	}
	
	public String toPrettyString() {
		StringBuilder builder = new StringBuilder();
		builder.append(name)
		.append(": ")
		.append(") ")
		.append(value)
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
}

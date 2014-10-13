package fr.eurecom.senml.entity;

import java.util.HashMap;
import java.util.List;
import java.util.logging.Logger;
import java.util.Map;

import javax.jdo.annotations.NotPersistent;
import javax.jdo.annotations.PersistenceCapable;
import javax.jdo.annotations.Persistent;
import javax.jdo.annotations.PrimaryKey;

import org.json.JSONObject;
import org.w3c.dom.Document;
import org.w3c.dom.Node;

import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;

import fr.eurecom.senml.persistence.JDOStorage;

/**
 * Represents the list of the monitored sensors, and the relative
 * minimum and maximum values (allowed range)
 */
@PersistenceCapable(detachable = "true")
public class SensorCheckAlarm {

	public enum ParamTypeValue {
		BOOL("bv"), FLOAT("v"), STRING("sv");

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
	private static final Logger log = Logger
			.getLogger("sensors.entity.SensorCheckAlarm");
	@NotPersistent
	public static final String PARAM_NAME = "n";
	@NotPersistent
	public static final String PARAM_VALUE = "v";
	@NotPersistent
	public static final String PARAM_STRING_VALUE = "sv";
	@NotPersistent
	public static final String PARAM_RT_ALLOWED = "rt-allowed";

	@PrimaryKey
	@Persistent
	Key key;
	
	@Persistent
	private String sensorKey;
	
	@Persistent
	private String sensorName;
	
	@Persistent
	private String zoneKey;
	
	// This should be an Object, and then through reflection setted to the correct type
	@Persistent
	private float minVal;
	
	@Persistent
	private float maxVal;

	public SensorCheckAlarm(String sensorKey, float minVal, float maxVal) {
		this.sensorKey = sensorKey;

		this.key = KeyFactory.createKey(SensorCheckAlarm.class.getSimpleName(),
				sensorKey + "sensoralarm");
		this.setMinVal(minVal);
		this.setMaxVal(maxVal);
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
			
			HashMap<String, Float> rtAllowed = new HashMap<String, Float>();
			rtAllowed.put("min", getMinVal());
			rtAllowed.put("max", getMaxVal());
			entrySenML.put("key", getKey());
			entrySenML.put("sensorkey", getSensorKey());
			entrySenML.put("sensorName", getParentSensorName());
			JSONObject rtAllowedJSON = new JSONObject(rtAllowed);
			entrySenML.put(PARAM_RT_ALLOWED, rtAllowedJSON);
			return entrySenML;
		} catch (Exception e) {
			e.printStackTrace();
			log.warning("Error Encoding a measure");
			return null;
		}
	}

	public Node toXMLSenML(Document d) {
		//TODO
		return null;
	}
	
	public String getParentSensorName() {
		SensorAdmin sensor = JDOStorage.getInstance().getById(sensorKey, SensorAdmin.class);
		return sensor.getTitle();
	}

	public String toString() {
		StringBuilder builder = new StringBuilder();
		builder.append("key: " + this.sensorKey).append(", [" + getMinVal() + ", ")
				.append(getMaxVal() + "]");
		return builder.toString();
	}

	public String getSensorKey() {
		return sensorKey;
	}
	
	public static List<SensorCheckAlarm> getAllMonitoredSensors() {
		return JDOStorage.getInstance().getAll(SensorCheckAlarm.class);
	}

	public float getMinVal() {
		return minVal;
	}

	public void setMinVal(float minVal) {
		this.minVal = minVal;
	}

	public float getMaxVal() {
		return maxVal;
	}

	public void setMaxVal(float maxVal) {
		this.maxVal = maxVal;
	}
}

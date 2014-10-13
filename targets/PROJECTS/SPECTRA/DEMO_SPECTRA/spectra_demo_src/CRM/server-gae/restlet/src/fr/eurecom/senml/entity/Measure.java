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
 * @author dalmassi
 * 
 */

@PersistenceCapable(detachable = "true")
public class Measure {

	public enum ParamTypeValue {
		BOOL("bv"), FLOAT("v"), STRING("sv"), SUM("s");

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
	private static final DateFormat df = new SimpleDateFormat(
			"yyyy.MM.dd G 'at' HH:mm:ss z");

	@NotPersistent
	private static final Logger log = Logger
			.getLogger("sensors.entity.Measure");
	@NotPersistent
	public static final String PARAM_UNIT = "u";
	@NotPersistent
	public static final String PARAM_NAME = "n";
	@NotPersistent
	public static final String PARAM_TIME = "t";
	@NotPersistent
	public static final String PARAM_VALUE = "v";
	@NotPersistent
	public static final String PARAM_STRING_VALUE = "sv";
	@NotPersistent
	public static final String PARAM_BOOLEAN_VALUE = "bv";
	@NotPersistent
	public static final String PARAM_SUM_VALUE = "s";
	@NotPersistent
	public static final String PARAM_UPDATE_TIME = "ut";
	@NotPersistent
	public static final String PARAM_TYPE = "type";

	@PrimaryKey
	@Persistent
	Key key;

	@Persistent
	private String name;

	@Persistent
	private String unitCS;

	@Persistent
	private String unitPrint;

	@Persistent
	private String unitKindOfQty;

	@Persistent
	private String value;

	@Persistent
	private String paramTypeValueEncoded;

	private long time;

	@Persistent
	private String sensorKey;

	private String baseunitCS;

	private String baseunitPrint;

	private String baseunitKindOfQty;

	private String type;

	private String sum;

	public Measure(String sensorKey, String name, Units unit,
			ParamTypeValue typeValue, String value, long time, String type) {
		this.sensorKey = sensorKey;

		this.name = name == null || name.isEmpty() ? generateName() : name;
		this.unitCS = unit == null ? null : unit.getCS();
		this.unitPrint = unit == null ? null : unit.getPrint();
		this.unitKindOfQty = unit == null ? null : unit.getKindofQty();
		this.paramTypeValueEncoded = typeValue.getSenML();
		this.value = value;
		this.time = time;
		this.key = KeyFactory.createKey(Measure.class.getSimpleName(),
				sensorKey + name);
		this.type = type;
	}
	
	public Measure(String sensorKey, String name, Units unit,
			ParamTypeValue typeValue, String value, long time, String type, String sum) {
		this.sensorKey = sensorKey;

		this.name = name == null || name.isEmpty() ? generateName() : name;
		this.unitCS = unit == null ? null : unit.getCS();
		this.unitPrint = unit == null ? null : unit.getPrint();
		this.unitKindOfQty = unit == null ? null : unit.getKindofQty();
		this.paramTypeValueEncoded = typeValue.getSenML();
		this.value = value;
		this.time = time;
		this.sum = sum;
		this.key = KeyFactory.createKey(Measure.class.getSimpleName(),
				sensorKey + name);
		this.type = type;

	}

	public void destroy() {
		JDOStorage.getInstance().delete(this);
	}

	public String getKey() {
		return key == null ? null : KeyFactory.keyToString(key);
	}
	
	public String getType() {
		return type;
	}

	public JSONObject toJSONSenML() {
		try {
			JSONObject entrySenML = new JSONObject();
			if (name != null) {
				entrySenML.put(PARAM_NAME, name);
			}
			
			if (sum != null) {
				entrySenML.put(PARAM_SUM_VALUE, sum);
			}
			
			entrySenML.put(PARAM_TYPE, type);

			entrySenML.put(PARAM_TIME, time);
			if (this.unitPrint != null) {
				entrySenML.put(PARAM_UNIT, unitCS);
			}

			if (paramTypeValueEncoded != null) {
				entrySenML.put(paramTypeValueEncoded, value);
			}
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
		measure.setAttribute(PARAM_TYPE, type);
		measure.setAttribute(PARAM_TIME, "" + time);
		if (this.unitPrint != null) {
			measure.setAttribute(PARAM_UNIT, unitCS);
		}
		
		if (this.sum != null) {
			measure.setAttribute(PARAM_SUM_VALUE, sum);
		}

		if (paramTypeValueEncoded != null) {
			measure.setAttribute(paramTypeValueEncoded, value);
		}
		return measure;
	}

	public String toString() {
		StringBuilder builder = new StringBuilder();
		if (this.unitCS != null) {
			builder.append("key[" + getKey()).append("] name[" + name)
					.append("] unitCS[" + unitCS)
					.append("] unitPrint[" + unitPrint)
					.append(this.sum == null ? "" : "] sum[" + sum)
					.append("] unitKindOfQty[" + unitKindOfQty)
					.append("] paramTypeEncoded[" + paramTypeValueEncoded)
					.append("] value[" + value).append("] time[" + time)
					.append("] sensorKey[" + sensorKey).append("]");
		} else {
			// Check if we have to initialize the base unit
			baseUnitInit();
			builder.append("key[" + getKey())
					.append("] name[" + name)
					.append("] unitCS (base unit) [" + baseunitCS)
					.append("] unitPrint (base unit) [" + baseunitPrint)
					.append(this.sum == null ? "" : "] sum[" + sum)
					.append("] unitKindOfQty (base unit) [" + baseunitKindOfQty)
					.append("] paramTypeEncoded[" + paramTypeValueEncoded)
					.append("] value[" + value).append("] time[" + time)
					.append("] sensorKey[" + sensorKey).append("]");
		}
		return builder.toString();
	}

	public String toPrettyString() {
		StringBuilder builder = new StringBuilder();
		if (this.unitCS != null) {
			builder.append(name).append(": ").append("(kind: ")
					.append(unitKindOfQty).append(") ").append(value)
					.append(this.sum == null ? "" : " sum: " + sum)
					.append(" ").append(unitPrint).append(" @ ")
					.append(formatDate(time));
		} else {
			// Check if we have to initialize the base unit
			baseUnitInit();
			builder.append(name).append(": ").append("(kind (baseunit): ")
					.append(baseunitKindOfQty).append(") ").append(value)
					.append(" ").append(baseunitPrint).append("(baseunit) @ ")
					.append(formatDate(time));
		}
		return builder.toString();
	}

	private void baseUnitInit() {
		System.out.println("unit is null, parent sensor is " + sensorKey);
		// Retrieve info about base unit from the parent sensor
		Sensor parent = JDOStorage.getInstance().getById(sensorKey,
				Sensor.class);
		System.out.println("sensor is " + parent.getTitle());
		String bu = parent.getBaseUnit();
		System.out.println("sensor bu is " + bu);
		Units baseUnits = Units.getUnit(bu);
		this.baseunitCS = baseUnits.getCS();
		this.baseunitKindOfQty = baseUnits.getKindofQty();
		this.baseunitPrint = baseUnits.getPrint();
		System.out.println("base unit is: " + this.baseunitCS);
	}

	private String formatDate(long ms) {
		if (ms == 0)
			return "now";
		if (ms < 0)
			return ("now - " + ms);

		return df.format(new Date(ms));
	}

	public String getSensorKey() {
		return sensorKey;
	}

	private String generateName() {
		return UUID.randomUUID().toString();
	}
}

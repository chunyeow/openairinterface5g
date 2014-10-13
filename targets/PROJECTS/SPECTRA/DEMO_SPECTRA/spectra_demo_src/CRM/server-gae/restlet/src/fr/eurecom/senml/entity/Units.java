package fr.eurecom.senml.entity;

import java.util.HashMap;
import java.util.Map;

/**
 * Units Definition from UCUM organization.
 * 
 * @author dalmassi
 * 
 */
public enum Units {
	ACIDITY("acidity", "pH", "pH"), // senml 10 Nota : ucum c/s = [pH]
	ACCELERATION("acceleration", "m/s2", "m/s2"), // senml 10
	AMPERE("electric current", "A", "A"),
	AREA("Area", "m2", "m2"), // senml 10
	BATTERY_LEVEL_PERCENT("Remaining percent battery", "%EL", "%EL"), // senml 10
	BATTERY_LEVEL_SECOND("Remaining seconds battery", "EL", "EL"), // senml 10
	BAR("pressure", "bar", "bar"),
	BEATS_PER_MINUTE("beats/minute", "beet/m", "beet/m"),  // senml 10 says beet/m, but ucum m = meter, min = minutes. This is confusing, also beet looks mispelled. Check next draft version.
	BEATS("Cumulative beats", "beets", "beets"), // senml 10.
	BECQUEREL("becquerel", "bq", "bq"),
	BEL_SOUND_LEVEL("bel sound pressure level", "Bspl", "Bspl"), // senml 10
	BIT_SECOND("bits per second", "bit/s", "bit/s"), // senml 10
	CANDELA("luminous intensity", "cd","cd"),
	COUNT("count", "count", "count"), // senml 10
	COULOMB("electric charge", "C", "C"), 
	DAY("time", "d", "d"),
	DEGREE_ANGLE("Plan Angle", "°", "deg"),
	DEGREE_CELSIUS("temperature", "°C", "Cel"), 
	DEGREE_FAHRENHEIT("temperature","°F", "[degF]"),
	FARAD("electric capacitance", "F", "F"), 
	GRAM("mass", "g", "g"),
	GRAM_PER_LITER("gram per liter", "g/L", "g/L"),
	GRAY("gray", "Gy", "Gy"), // senml 10
	KATAL("katal", "kat", "kat"), // senml 10
	KELVIN("temperature", "K", "K"),
	HENRY("henry", "H", "H"), // senml 10
	HERTZ("hertz", "Hz", "Hz"),
	HUMIDITY_RELATIVE("humidity", "%RH", "%RH"), // senml 10
	HOUR ("time", "h", "h"),
	IRRIDIANCE("irridiance", "W/m2", "W/m2"), // senml 10
	JOULE("joule", "J", "J"),
	KILO_GRAM("mass", "kg", "kg"), // senml 10
	KILO_WATT_HOUR("kilo watt per hour", "kWh", "kW/h"),
	LATITUDE("WGS84-Latitude", "lat", "lat"), // draft senml 10  
	LITER("volume", "l", "l"),
	LITER_SECOND("flow liters per seconds", "l/s", "l/s"), // senml 10 Nota ucum = L/s
	LUMEN("luminous flux", "lm", "lm"), 
	LUMINANCE("luminance", "cd/m2", "cd/m2"), // senml 10
	LONGITUDE("WGS84-Longitude", "lon", "lon"), // draft senml 10
	LUX("illuminance", "lx", "lx"),   
	METER("length", "m", "m"), 
	MINUTE_ANGLE("Plan angle", "′", "′"), 
	MINUTE_TIME("time", "min", "min"),
	MOLE("mole", "mol", "mol"), // senml 10
	NEWTON("newton", "N", "N"), // senml 10
	OHM("electric resistance", "Ω","Ohm"),
	PASCAL("pressure", "Pa", "Pa"), // draft senml 10
	PERCENT("percent", "%", "UCUM:%"), // ucum 
	RADIAN("plane angle", "rad", "rad"),
	SECOND_ANGLE("Plan Angle", "″", "″"),
	SECOND_TIME("time", "s", "s"),
	SIEMENS("Siemens", "S", "S"), // senml 10
	SIEVERT("Sievert", "Sv", "Sv"), // senml 10
	STERADIAN("steradian", "sr", "sr"), // senml 10
	SWITCH("switch", "switch", "%"), // senml 10 conflict with ucum percent
	TONNE("mass", "t", "t"),  
	TESLA("magnetic flux density", "T", "T"),
	VELOCITY("velocity", "m/s", "m/s"),
	VOLT("electric potential", "V", "V"), 
	WATT("power", "W", "W"),
	WEBER("wever", "Wb", "Wb"), // senml 10
	YEAR("time", "a", "a");

	private final String print;
	private final String cs;
	private final String kindOfQty;
	
	private final static Map<String, Units> csMap = new HashMap<String, Units>();

	Units(String kind, String print, String cs) {
		this.print = print;
		this.cs = cs;
		this.kindOfQty = kind;
	}

	public String getCS() {
		return cs;
	}

	public String getKindofQty() {
		return kindOfQty;
	}

	public String getPrint() {
		return print;
	}
	
	public static Units getUnit(String cs) {
		if (csMap.size() == 0) {
			initCsMap();
		}
		return csMap.get(cs);
	}
	
	private static synchronized void initCsMap() {
		if (csMap.size() > 0 ) {
			return;
		}
		for (Units u : values()) {
			csMap.put(u.cs, u);
		}
	}
}

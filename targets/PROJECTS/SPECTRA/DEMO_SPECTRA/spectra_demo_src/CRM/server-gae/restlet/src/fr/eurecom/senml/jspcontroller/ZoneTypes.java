package fr.eurecom.senml.jspcontroller;

public class ZoneTypes {
	
	// Refers to
	// http://www.sensormeasurement.appspot.com/json/m3/featureOfInterest
	private static String[] zoneTypes = {"City", "Environment", "Military",
			"Tracking", "Health", "Place", "Building", "Transportation",
			"BrainWave"};

	public static String[] getZoneTypes() {
		return zoneTypes;
	}
}

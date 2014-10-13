package fr.eurecom.senml.entity;

import java.util.List;

import org.json.JSONObject;
import org.w3c.dom.Document;
import org.w3c.dom.Node;

public interface ISensor {
	public final static String BASE_NAME = "bn";
	public final static String BASE_TIME = "bt";
	public final static String BASE_UNIT = "bu";
	public final static String VERSION = "ver";
	public final static String MEASUREMENT = "e";
			
	public String getUUID();
		
	public IZone getZone();

	public String getTitle();
	
	public List <Measure> getMeasures();
	
//	public List<String> getMeasuresKey();
	
//	public List<String> getMeasuresPrint();
	
	public JSONObject toJSONSenML();
	
	public Document toXMLSenML();
	
	public Node toXMLSenML(Document rootDocument);

	public String getBaseUnit();

	public String zoneType();
}

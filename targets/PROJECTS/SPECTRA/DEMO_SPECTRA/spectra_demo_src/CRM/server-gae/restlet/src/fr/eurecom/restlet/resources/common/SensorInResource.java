package fr.eurecom.restlet.resources.common;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.restlet.data.MediaType;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.ext.wadl.MethodInfo;
import org.restlet.ext.wadl.RepresentationInfo;
import org.restlet.ext.wadl.ResponseInfo;
import org.restlet.ext.wadl.WadlServerResource;
import org.restlet.representation.Representation;
import org.restlet.resource.Get;
import org.restlet.resource.Delete;
import org.restlet.resource.Put;
import org.restlet.resource.Post;

import fr.eurecom.senml.entity.*;
import fr.eurecom.senml.persistence.JDOStorage;


public class SensorInResource extends WadlServerResource {
	private static final Logger log = Logger.getLogger("SensorInResource");
	private static final MediaType SENML_JSON = MediaType.valueOf("application/senml+json");
	/**
	 * {@inheritDoc}
	 */
	@Override
	public Representation describe() {
		setName("Sensor-In  Resource");
		setDescription("Acquisition sensors receiver");
		return super.describe();
	}

	@Post
	@Put
	public void updateSensor(Representation entity) {
		JSONObject jObject = null;
		String text = null;
		try {
			MediaType mt = entity.getMediaType();
			text = entity.getText();
			if (mt.isCompatible(MediaType.APPLICATION_JSON) || mt.isCompatible(SENML_JSON) ) {
				jObject = new JSONObject(text);
			}
			else {
				setStatus(Status.CLIENT_ERROR_NOT_ACCEPTABLE, "Content is not JSON compatible");
	        	 log.info("This entry cause a 406 response  / reason not json");
				return;
			}
         
	         String bn = jObject.has(ISensor.BASE_NAME) ? jObject.getString(ISensor.BASE_NAME) : null;
	         long bt = jObject.has(ISensor.BASE_TIME) ? jObject.getLong(ISensor.BASE_TIME) : Long.MIN_VALUE;
	         String bu = jObject.has(ISensor.BASE_UNIT) ? jObject.getString(ISensor.BASE_UNIT) : null;
	         
	         if (! jObject.has(ISensor.MEASUREMENT) || bn == null) {
	        	 setStatus(Status.CLIENT_ERROR_BAD_REQUEST, "Parameter Base Name[bn] or measure[e] missing.");
	        	 log.info("This entry cause a 400 response. reason bn - e missing ||" + text + "||");
	        	 return;
	         }
	         JSONArray measures = jObject.getJSONArray(ISensor.MEASUREMENT);

// TODO: Remove this hack: zone key is static. For first tests with IQSIM and demo. 	         
	         IZone zone = JDOStorage.getInstance().getById("Aix", ZoneAdmin.class);
	         SensorAdmin sa = new SensorAdmin(bn, "Generic Sensor", zone);
	         boolean parsingOK = true;
	         String reasonPhrase = null;
	         
	         for (int i = 0; i < measures.length(); i++) {
	        	 
	        	 JSONObject m = measures.getJSONObject(i);
	        	 
	        	 Measure.ParamTypeValue tv = getUnitType(m);
	        	 if (tv == null) {
	        		 parsingOK = false;
	        		 reasonPhrase = "Measure miss type value [v|sv|bv]";
	        		 break;
	        	 }
	        	 
	        	 Units unit = getUnit(m, bu);
	        	 if (unit == null) {
	        		 parsingOK = false;
	        		 reasonPhrase = "parameter unit [bu|u] invalid (missing or unknown)";
	        		 break;
	        	 }
	        	 
	        	 String name = m.has(Measure.PARAM_NAME) ? m.getString(Measure.PARAM_NAME) : null;
	        	 if ( name == null || name.trim().isEmpty()) {
	        		 parsingOK = false;
	        		 reasonPhrase = "parameter name [n] missing or empty. Not supported";
	        		 break;
	        	 }
	        	 
	        	 String type = m.has(Measure.PARAM_TYPE) ? m.getString(Measure.PARAM_TYPE) : null;
	        	 if ( type == null || type.trim().isEmpty()) {
	        		 parsingOK = false;
	        		 reasonPhrase = "parameter type [n] missing or empty. Not supported";
	        		 break;
	        	 }

	        			 
	        	 sa.addMeasure(name.trim(), unit, tv,  
	        			 m.getString(tv.getSenML()), 
	        			 m.has(Measure.PARAM_TIME) ? m.getLong(Measure.PARAM_TIME) : 
	        				 bt == Long.MIN_VALUE ? 0 : bt, type.trim());
	         }
	         
	         if (!parsingOK) {
	        	 setStatus(Status.CLIENT_ERROR_BAD_REQUEST, reasonPhrase);
	        	 log.info("This entry cause a 400 response / reason " + reasonPhrase + " ||" + text + "||");
	         }
	         else {
	        	 setStatus(Status.SUCCESS_CREATED);
	         }
 		}
		catch (Exception e) {
			log.warning("Unexpected error receiving this entry : ||" + text + "||");
			log.log(Level.WARNING, "Unexpected Error", e);
			
			setStatus(Status.SERVER_ERROR_INTERNAL, "Unexpected Error");
		}
		return;
	}
	
	private static Measure.ParamTypeValue getUnitType(JSONObject m) {
		if (m.has(Measure.PARAM_VALUE)) {
			return Measure.ParamTypeValue.FLOAT;
		}
		else if (m.has(Measure.PARAM_BOOLEAN_VALUE)) {
			return Measure.ParamTypeValue.BOOL;
		}
		else if (m.has(Measure.PARAM_STRING_VALUE)) {
			return Measure.ParamTypeValue.STRING;
		}
		else if (m.has(Measure.PARAM_SUM_VALUE)) {
			return Measure.ParamTypeValue.SUM;
		}
		return null;
	}
	
	private static Units getUnit(JSONObject m, String bu) throws JSONException {
		String u = m.has(Measure.PARAM_UNIT) ? m.getString(Measure.PARAM_UNIT) : bu != null ? bu : null;
		return u == null ? null : Units.getUnit(u);
	}
}
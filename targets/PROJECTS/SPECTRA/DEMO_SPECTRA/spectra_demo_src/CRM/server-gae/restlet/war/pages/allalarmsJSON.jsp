<%@ page trimDirectiveWhitespaces="true" %>
<%-- Set the content type header with the JSP directive --%>
<%@ page contentType="application/json"%>
<%-- Set the content disposition header --%>
<%response.setContentType("application/json");%>
<%@ page language="java"%>
<%@ page import="java.util.List"%>
<%@ page import="java.util.Iterator"%>
<%@ page import="org.json.JSONArray"%>
<%@ page import="org.json.JSONObject"%>
<%@ page import="fr.eurecom.senml.entity.SensorAdmin"%>
<%@ page import="fr.eurecom.senml.entity.Measure"%>
<%@ page import="fr.eurecom.senml.entity.SensorCheckAlarm"%>
<%@ page import="fr.eurecom.senml.persistence.JDOStorage"%>

	<%
		JSONArray array = new JSONArray();
		List<SensorCheckAlarm> list = SensorCheckAlarm.getAllMonitoredSensors();
		Iterator<SensorCheckAlarm> iter = list.iterator();
		while (iter.hasNext()) {
			SensorCheckAlarm s = iter.next();
			SensorAdmin sensor = JDOStorage.getInstance().getById(s.getSensorKey(), SensorAdmin.class);
			JSONObject element = new JSONObject();
			element.put("name", s.getParentSensorName());
			element.put("baseunit", sensor.getBaseUnit());
			element.put("monitor", s.toJSONSenML());
			
			/* TODO: REMARK
			CORELINKS will introduce intelligent sensors.
			At that time, we'll be able to have different measures of different types
			from the same sensor.
			FOR NOW, 1 sensor corresponds to 1 measure of the same type, everytime!
			*/
			
			// Retrieve value
			List<Measure> measures = sensor.getMeasures();
			if (measures.isEmpty()) {
				System.out.println("Measures empty!");
			} else {
				element.put("measure", measures.get(0).toJSONSenML());
				array.put(element);
			}
		}
	%>
<%= array %>
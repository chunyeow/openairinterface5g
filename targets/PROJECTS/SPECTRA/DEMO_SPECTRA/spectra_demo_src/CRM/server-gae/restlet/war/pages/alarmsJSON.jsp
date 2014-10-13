<%@ page trimDirectiveWhitespaces="true" %>
<%-- Set the content type header with the JSP directive --%>
<%@ page contentType="application/json"%>
<%-- Set the content disposition header --%>
<%response.setContentType("application/json");%>
<%@ page language="java"%>
<%@ page import="java.util.List"%>
<%@ page import="java.util.Iterator"%>
<%@ page import="org.json.JSONArray"%>
<%@ page import="fr.eurecom.senml.entity.SensorCheckAlarm"%>
<%@ page import="fr.eurecom.senml.persistence.JDOStorage"%>

	<%
		JSONArray array = new JSONArray();
		List<SensorCheckAlarm> list = SensorCheckAlarm.getAllMonitoredSensors();
		Iterator<SensorCheckAlarm> iter = list.iterator();
		while (iter.hasNext()) {
			SensorCheckAlarm s = iter.next();
			array.put(s.toJSONSenML());
		}
	%>
<%= array %>
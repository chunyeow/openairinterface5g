<%@ page trimDirectiveWhitespaces="true" %>
<%-- Set the content type header with the JSP directive --%>
<%@ page contentType="application/json"%>
<%-- Set the content disposition header --%>
<%response.setContentType("application/json");%>
<%@ page language="java"%>
<%@ page import="fr.eurecom.senml.entity.SensorCheckAlarm"%>
<%@ page import="fr.eurecom.senml.persistence.JDOStorage"%>
<% 
		String sensorKey = request.getParameter("key");
		SensorCheckAlarm s = JDOStorage.getInstance().getById(sensorKey, SensorCheckAlarm.class);
%>
<%= s.toJSONSenML() %>
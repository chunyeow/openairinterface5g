<%@ page contentType="text/html;charset=UTF-8" language="java"%>
<%@ page import="fr.eurecom.senml.entity.IZone"%>
<%@ page import="fr.eurecom.senml.entity.IZoneAdmin"%>
<%@ page import="fr.eurecom.senml.entity.ISensor"%>
<%@ page import="fr.eurecom.senml.entity.SensorCheckAlarm"%>
<%@ page import="java.util.List"%>
<%@ page import="java.util.Iterator"%>
<html>
<head>
<link href="/css/template_style.css" type="text/css" rel="stylesheet" />
<script src="/js/main.js" type="text/javascript">
	
</script>
<title>Alarms</title>
</head>

<body>
<div id="template_main">
	<table border="1">
	<tr><th>Name</th><th>Min value</th><th>Max value</th><th>JSON</th><th>Change</th><th>Remove</th></tr>
	
	<%
		List<SensorCheckAlarm> list = SensorCheckAlarm.getAllMonitoredSensors();
		Iterator<SensorCheckAlarm> iter = list.iterator();
		while (iter.hasNext()) {
			SensorCheckAlarm s = iter.next();
	%>
	<tr>
	<td>
		<strong><%= s.getParentSensorName() %></strong></td>
	<td> <%= s.getMinVal() %></td>
	<td> <%= s.getMaxVal() %></td>
	<td><a href="getmonitor?key=<%= s.getKey() %>">JSON</a></td>
	<td><a href="edit">Edit</a></td>
	<td><a href="removemonitor?key=<%= s.getKey() %>">Remove</a></td>
	</tr>
	<% 	} %>
	</table>
	</div>

</body>
<footer id="template_footer">Eurecom / background pattern draw
	by vectorpile.com</footer>
</html>
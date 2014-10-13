<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="fr.eurecom.senml.entity.IZone" %>
<%@ page import="fr.eurecom.senml.jspcontroller.ZoneTypes" %>
<%@ page import="java.util.List" %>
<html>
<head>
<link href="../css/template_style.css" type="text/css" rel="stylesheet"/>
<title>Zone Main page</title>
</head>

<body>
	<div id="template_header">
	<h1>Supervisor Page</h1>
	<br/><br/>
	<hr>
	<h4> New Zone</h4>
	<form action="addZone" method="post" id="form-add">
		<p>
			<input type="text" name="nameZone" 
				placeholder="name zone" required="required">
			<input type="text" name="parentNameZone" 
				placeholder=" parent zone name"> 
		</p>
		<p>Choose a type: <select name="typeZone">
			<% String[] listZones = ZoneTypes.getZoneTypes();
			   for (int i = 0; i < listZones.length; i++) {
			%>
				<option value="<%= listZones[i] %>"> <%= listZones[i] %>
			<%
			   }
			%>
			</select>
			</p>
		<p><input type="submit" value="Save"> <input type="reset" value="Clear"></p>
	</form>
	<hr>	
	</div>	
	
	<br/><br/>
	<div id="template_main">

	<table id="zone-table">
 	<caption>Full list</caption> 
		<thead> 
		<tr> <th>Name</th> <th>Parent</th><th>Type</th><th></th><th></th> </tr>	
		</thead>
		<tbody>
		
<%		List<IZone> zones = (List<IZone>)request.getAttribute("zoneList");
		if (zones != null && !zones.isEmpty()) {
			for (IZone z : zones) {
%>
			<tr>				
				<td> <%= z.getName() %></td>
				<td> <%= z.getParentZone() == null ? "" : z.getParentZone().getName() %></td>
				<td> <%= z.getType() %></td>
				<td><a href="<%= z.getKey() %>/edit">Edit</a></td>
				<td><a href="deleteZone/<%= z.getKey() %>">Delete</a></td>
			</tr>
<% 				  
			}
 		}
%>
	</tbody>
	<tfoot><tr> <td></td><td></td><td></td><td></td> </tr></tfoot>
	</table>
	</div>

</body>
<footer id="template_footer">Eurecom / background pattern draw by vectorpile.com</footer>
</html>
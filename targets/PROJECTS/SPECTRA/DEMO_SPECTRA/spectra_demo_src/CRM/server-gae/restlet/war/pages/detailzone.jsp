<%@ page contentType="text/html;charset=UTF-8" language="java"%>
<%@ page import="fr.eurecom.senml.entity.IZone"%>
<%@ page import="fr.eurecom.senml.entity.IZoneAdmin"%>
<%@ page import="fr.eurecom.senml.entity.ISensor"%>
<%@ page import="fr.eurecom.senml.entity.Measure"%>
<%@ page import="fr.eurecom.senml.entity.Units"%>
<%@ page import="fr.eurecom.senml.jspcontroller.ZoneTypes"%>
<%@ page import="java.util.List"%>
<html>
<head>
<link href="/css/template_style.css" type="text/css" rel="stylesheet" />
<script src="/js/main.js" type="text/javascript">
	
</script>
<title>Zone Detail page</title>
</head>

<body>
	<%
		IZoneAdmin z = (IZoneAdmin) request.getAttribute("zone");
		List<IZone> subZones = z.getSubZones();
		List<ISensor> sensors = z.getSensors();
		Units[] units = Units.values();
	%>
	<div id="template_header">
		<h1>
			Zone:
			<%=z.getName()%>
			in:
			<%=z.getParentZone() == null ? "" : z.getParentZone()
					.getName()%>
		</h1>
		<a href="../zones"> Zones Full List </a>
		<hr>
	</div>

	<div id="template_main">
		<!--  subzones -->
		<form action="addsubZone" method="post" id="zone_form">
			<label>New sub zone</label> <input type="text" name="nameSubZone"
				placeholder="nameZone" required="required"> <input
				type="hidden" name="zoneKey" value="<%=z.getKey()%>">

			<p>
				Choose a type: <select name="typeZone">
					<%
						String[] listZones = ZoneTypes.getZoneTypes();
						for (int i = 0; i < listZones.length; i++) {
					%>
					<option value="<%=listZones[i]%>">
						<%=listZones[i]%>
						<%
							}
						%>
					
				</select>
			</p>
			<input type="submit" value="Add Sub Zone">
		</form>

		<table id="zone-table">
			<caption>Sub Zones</caption>
			<thead>
				<tr>
					<th>Name</th>
					<th>SubZones</th>
					<th>Sensors</th>
					<th></th>
					<th></th>
				</tr>
			</thead>

			<tbody>
				<%
					for (IZone sz : subZones) {
				%>
				<tr>
					<td><%=sz.getName()%></td>
					<td><%=sz.getSubZones().size() > 0 ? "Yes" : "No"%></td>
					<td><%=sz.getSensors().size() > 0 ? "Yes" : "No"%></td>
					<td><a href="../<%=sz.getKey()%>/edit">Edit</a></td>
					<td><a href="delete/<%=sz.getKey()%>">Delete</a></td>
				</tr>
				<%
					}
				%>
			</tbody>
			<tfoot>
				<tr>
					<td></td>
					<td></td>
					<td></td>
					<td></td>
					<td></td>
				</tr>
			</tfoot>
		</table>

		<!--  sensors -->
		<br> <br>
		<form action="addSensor" method="post" id="zone_form">
			<label>New Sensor</label> <input type="text" name="title"
				placeholder="title sensor" required="required"> <br /> <label
				for="baseunit">Base unit</label> <select name="baseunit">

				<%
					for (Units u : units) {
				%>
				<option value="<%=u.getCS()%>"><%=u.name().toLowerCase()%></option>
				<%
					}
				%>
			</select> <input type="hidden" name="zoneKey" value="<%=z.getKey()%>">
			<br />
			<p>
				<select name="typeSensor">
					<option value="Magnetometer Sensor">Magnetometer Sensor</option>
					<option value="GPS Sensor">GPS Sensor</option>
					<option value="Gyrometer Sensor">Gyrometer Sensor</option>
					<option value="Optical Sensor">Optical Sensor</option>
					<option value="Gyroscope Sensor">Gyroscope Sensor</option>
					<option value="Hygrometer">Hygrometer</option>
					<option value="Conductivity Sensor">Conductivity Sensor</option>
					<option value="Wind Direction Sensor">Wind Direction
						Sensor</option>
					<option value="Pluviometer">Pluviometer</option>
					<option value="Thermometer">Thermometer</option>
					<option value="Light Sensor">Light Sensor</option>
					<option value="Smoke Detector">Smoke Detector</option>
					<option value="ThermalSensor">ThermalSensor</option>
					<option value="Respiration">Respiration</option>
					<option value="Electromagnetic Sensor">Electromagnetic
						Sensor</option>
					<option
						value="Solar Radiation Sensor. PAR (Photosynthetically Active Radiation) Sensor">Solar
						Radiation Sensor. PAR (Photosynthetically Active Radiation) Sensor</option>
					<option value="Pressure Sensor">Pressure Sensor</option>
					<option value="Pollutant Sensor">Pollutant Sensor</option>
					<option value="Pulse Oxymeter">Pulse Oxymeter</option>
					<option value="Wind Velocity Sensor">Wind Velocity Sensor</option>
					<option value="Mechanical Sensor">Mechanical Sensor</option>
					<option value="ElectricalSensor">ElectricalSensor</option>
					<option value="Acoustic Sensor">Acoustic Sensor</option>
					<option value="Motion Sensor/ Accelerometer">Motion
						Sensor/ Accelerometer</option>
					<option value="Clock">Clock</option>
					<option value="Pyroelectric IR Occupancy Detector">Pyroelectric
						IR Occupancy Detector</option>
					<option value="Chemical Sensor">Chemical Sensor</option>
					<option value="Nuclear Sensor">Nuclear Sensor</option>
				</select>
			</p>
			<input type="submit" value="Add Sensor">
		</form>

		<table id="zone-table">
			<caption>Sensors</caption>
			<thead>
				<tr>
					<th>Title</th>
					<th>UUID</th>
					<th>Type</th>
					<th>Base Unit</th>
					<th>Measures</th>
					<th></th>
					<th></th>
					<th></th>
				</tr>
			</thead>

			<tbody>
				<%
					for (ISensor s : sensors) {
				%>
				<tr id="sensor">
					<td><%=s.getTitle()%></td>
					<td><%=s.getUUID()%></td>
					<td><%=s.zoneType()%></td>
					<%
						if (s.getBaseUnit() != null) {
					%>
					<td><%=s.getBaseUnit()%></td>
					<%
						} else {
					%>
					<td>No base unit</td>
					<%
						
						}
					String formID = "measureAdd" + s.getUUID();
					String valueID = "value" + s.getUUID();
					%>

					<td><form action="addMeasure" method="post" id="<%= formID %>"
							name="<%= formID %>">
							<fieldset>
								<legend>Add the sensor to the list of monitored
									sensors</legend>
									<input name="rtallowedmin" placeholder="min value allowed"/><br/>
									<input name="rtallowedmax" placeholder="max value allowed"/>
									<input name="bn" type="hidden" value="<%= s.getTitle() %>"/>
								<button onClick="registerForAlert('<%=s.getUUID()%>');">Monitor!</button>
							</fieldset>

							<p>
								Type: <select name="measureType">
									<option value="Animal">Animal</option>
									<option value="Paymend Card">Payment Card</option>
									<option value="Luggage">Luggage</option>
									<option value="Passport">Passport</option>
									<option value="Clothing">Clothing</option>
									<option value="ParkingSpace">ParkingSpace</option>
									<option value="Book">Book</option>
									<option value="Food">Food</option>
									<option value="Toll">Toll</option>
									<option value="CD">CD</option>
									<option value="DVD">DVD</option>
									<option value="TransitPass">TransitPass</option>
									<option value="Sodium">Sodium</option>
									<option value="Conductivity">Conductivity</option>
									<option value="Visibility">Visibility</option>
									<option value="LuminousFlux">LuminousFlux</option>
									<option value="MagneticFluxDensity">MagneticFluxDensity</option>
									<option value="HeartBeat">HeartBeat</option>
									<option value="Glucose">Glucose</option>
									<option value="Oxygen">Oxygen</option>
									<option value="Calcium">Calcium</option>
									<option value="Luminous Intensity">Luminous Intensity</option>
									<option value="Blood pressure">Blood pressure</option>
									<option value="Wind Direction">Wind Direction</option>
									<option value="Angular">Angular</option>
									<option value="Electrical Potential">Electrical
										Potential</option>
									<option value="Temperature">Temperature</option>
									<option value="Motion">Motion</option>
									<option value="Electric Charge">Electric Charge</option>
									<option value="Illuminance">Illuminance</option>
									<option value="Electric Current">Electric Current</option>
									<option value="Cholesterol">Cholesterol</option>
									<option value="Atmospheric Pressure">Atmospheric
										Pressure</option>
									<option value="Power">Power</option>
									<option value="Capacitance">Capacitance</option>
									<option value="Potassium">Potassium</option>
									<option value="RFID Measurement Type">RFID Measurement
										Type</option>
									<option value="Electrical Resistance">Electrical
										Resistance</option>
									<option value="Humidity">Humidity</option>
									<option value="Skin conductance/ GSR">Skin
										conductance/ GSR</option>
									<option value="Location">Location</option>
									<option value="Magnetic Field">Magnetic Field</option>
									<option value="Precipitation">Precipitation</option>
									<option value="Pressure">Pressure</option>
									<option
										value="PAR Measurement (Photosynthetically Active Radiation)">PAR
										Measurement (Photosynthetically Active Radiation)</option>
									<option value="Water level">Water level</option>
									<option value="Wind Velocity">Wind Velocity</option>
								</select>
							</p>
							<input type="hidden" name="sensorKey" value="<%=s.getUUID()%>">
							<input type="hidden" name="bu" value="<%=s.getBaseUnit()%>">
							Name: <input type="text" name="name" placeholder="measure name">
							<%
								if (s.getBaseUnit() != null) {
							%>
							<br />
							<p>
							<fieldset>
								<legend>If not specified, the value is in the base
									unit:</legend>
								<%=s.getBaseUnit()%></p>
								<p>
									Specify value type: <input type="checkbox" name="specmeasure"
										onClick="toggleDOMEl('<%=s.getUUID()%>measurediv');">
								</p>
								<div id="<%=s.getUUID()%>measurediv" class="hidden">
									<%
									} else {
								%>
									<div id="<%=s.getUUID()%>measurediv">
										<%
										}
									%>
										<select name="unit">
											<%
											for (Units u : units) {
										%>
											<option value="<%=u.getCS()%>"><%=u.name().toLowerCase()%></option>
											<%
											}
										%>
										</select>
							</fieldset>

							</div>
							<br /> When: <select name="time">
								<option value="NOW">now</option>
								<option value="AUTO">auto</option>
							</select> <br /> <br />
							<fieldset>
								<legend>Value</legend>
								Value type: <select name="typeValue">
									<option value="FLOAT">float</option>
									<option value="BOOL">boolean</option>
									<option value="String">String</option>
								</select> Value: <input type="text" name="value" id="<%=  valueID %>" placeholder="value"
									required="required">
							</fieldset>
							<br /> <label for="version">Version</label> <select
								name="version">
								<option value="2.0">2.0</option>
								<option value="1.0">1.0</option>
							</select> <br /> <br /> Sum: <input type="text" placeholder="sum"
								name="sum" /> <br /> <br /> <input type="submit"
								value="add a Measure">
						</form></td>
					<td></td>
					<td></td>
					<td><a href="deleteSensor/<%=s.getUUID()%>">Delete</a></td>
				</tr>
				<%
					List<Measure> measures = s.getMeasures();
						for (Measure m : measures) {
				%>
				<tr id="measure">
					<td></td>
					<td><%=m.toPrettyString()%></td>
					<td><%=m.toJSONSenML().toString()%>
					<td></td>
					<td></td>
					<td><a href="deleteMeasure/<%=m.getKey()%>">Delete</a></td>
				</tr>
				<%
					}
					}
				%>
			</tbody>
			<tfoot>
				<tr>
					<td></td>
					<td></td>
					<td></td>
					<td></td>
					<td></td>
					<td></td>
				</tr>
			</tfoot>
		</table>
	</div>

</body>
<footer id="template_footer">Eurecom / background pattern draw
	by vectorpile.com</footer>
</html>
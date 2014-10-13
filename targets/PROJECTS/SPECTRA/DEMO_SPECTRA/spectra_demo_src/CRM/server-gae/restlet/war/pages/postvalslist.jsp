<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="fr.eurecom.senml.entity.PostValType" %>
<%@ page import="java.util.List" %>
<html>
<head>
<link href="../css/template_style.css" type="text/css" rel="stylesheet"/>
<title>PostVal Main page</title>
</head>

<body>
	<br/><br/>
	<div id="template_main">

	<table id="zone-table">
 	<caption>Full list</caption> 
		<thead> 
		<tr> <th>Name</th> <th>Value</th> </tr>	
		</thead>
		<tbody>
		
<%		List<PostValType> postvals = (List<PostValType>)request.getAttribute("postvalsList");
		if (postvals != null && !postvals.isEmpty()) {
			for (PostValType z : postvals) {
%>
			<tr>				
				<td> <%= z.getName() %></td>
				<td> <%= z.getValue() %> @ <%= z.getTime() %></td>
			</tr>
<% 				  
			}
 		}
%>
	</tbody>
	<tfoot><tr> <td></td><td></td> </tr></tfoot>
	</table>
	</div>

</body>
<footer id="template_footer">Eurecom / background pattern draw by vectorpile.com</footer>
</html>

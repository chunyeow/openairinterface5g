package fr.eurecom.pushing;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.UUID;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.mortbay.log.Log;
import org.restlet.resource.Get;
import fr.eurecom.senml.entity.RegisteredDeviceType;
import fr.eurecom.senml.persistence.JDOStorage;

/**
 * Exposes the following URLs:
 *  - /pushing/register, POST, to register a device
 *  - /pushing/unregister, POST, to unregister a device
 *  - /pushing/list, GET, to retrieve the list of registered devices
 *  - /pushing/test, GET, test a notification to the registered devices
 *
 */
public class DeviceManager extends HttpServlet {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private static final Logger log = Logger.getLogger("DeviceManager");	
	
	public void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException {
		System.out.println("OK doPost");
		System.out.println(request.getParameter("reqType") + " " + request.getParameter("deviceType"));
		try {
			if (request.getParameter("reqType").equals("reg")) {
				// Registration of a device. Android or iOS?
				if (request.getParameter("deviceType").equals("android")) {
					registerAndroidDevice(request, response);
				} else if (request.getParameter("deviceType").equals("ios")) {
					registerAppleDevice(request, response);
				}
			} else if (request.getParameter("reqType").equals("unreg")) {
				System.out.println("OK UNREGISTERING!");
				if (request.getParameter("deviceType").equals("android")) {
					System.out.println("UNREGISTERING ANDROID!");
					unregisterAndroidDevice(request, response);
				} else if (request.getParameter("deviceType").equals("ios")) {
					unregisterAppleDevice(request, response);
				}
			}
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			response.setStatus(500);
		}
	}
	

	private void unregisterAppleDevice(HttpServletRequest request,
			HttpServletResponse response) {
		// TODO Auto-generated method stub
		response.setStatus(403);
	}


	private void unregisterAndroidDevice(HttpServletRequest request,
			HttpServletResponse response) {
		String key = request.getParameter("registrationID");
		// TODO Auto-generated method stub
		List<RegisteredDeviceType> devices = JDOStorage.getInstance().getAll(RegisteredDeviceType.class);
		Iterator<RegisteredDeviceType> iter =  devices.iterator();
		while (iter.hasNext()) {
			RegisteredDeviceType device = iter.next();
			System.out.print("Analyzing " + device.getDeviceRegistrationKey());
			if (device.getDeviceRegistrationKey().equals(key)) {
				JDOStorage.getInstance().delete(device);
				System.out.println("OK");
				return;
			} else {
				System.out.println("NO!");
			}
		}
	}

	private void registerAppleDevice(HttpServletRequest request, HttpServletResponse response) {
		// TODO Auto-generated method stub
		response.setStatus(403);
	}

	private void registerAndroidDevice(HttpServletRequest request, HttpServletResponse response) throws JSONException, IOException {
		Date n = new Date();
		
		boolean isadmin = (request.getParameter("isadmin") != null);
		
		
		
		RegisteredDeviceType device = new RegisteredDeviceType(
					UUID.randomUUID().toString(),
					request.getParameter("deviceType") + ": " + request.getParameter("deviceDet"),
					request.getParameter("devicekey"), n.getTime());
		if (isadmin) {
			System.out.println("ADMIN");
			device.setIsadmin(isadmin);
		}
		JDOStorage.getInstance().write(device);
		response.setContentType("text/html");
        PrintWriter writer = response.getWriter();
        response.setStatus(200);
        writer.write("Registered");
	}
	
	/**
	 * Handle HTTP GET method / Json
	 * 
	 * @return a JSON Zone Representation
	 */
	@Get
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException, ServletException {
		PrintWriter writer = response.getWriter();
		if (request.getPathInfo().equals("/test")) {
			GCMPushingServer GCM = GCMPushingServer.getInstance();
			try {
				GCM.sendNotification("notification test ");
				writer.write("Sent a notification test to the following devices:");
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else if (request.getPathInfo().equals("/alarm")) {
			GCMPushingServer GCM = GCMPushingServer.getInstance();
			try {
				GCM.sendNotification("ALARM for sensor: " + request.getParameter("sensor"));
				writer.write("OK");
			} catch (JSONException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				writer.write("NO");
			}
			return;
		}
		List<RegisteredDeviceType> devices = JDOStorage.getInstance().getAll(RegisteredDeviceType.class);
		if (devices.isEmpty()) {
			// No devices registered
			response.setStatus(200);
			writer.write("The list of devices is empty.");
			return;
		}
		response.setContentType("text/html");
		JSONArray jsonDevices = new JSONArray();
		Iterator<RegisteredDeviceType> iter = devices.iterator();
		writer.write("<table border=1><tr style=\"font-weight: bold;\"><th>Device Logo</th><th>Device Type</th><th>registered on</th><th>Registration Key</th></tr>");
		// Get the list of devices and build the json
		try {
			while (iter.hasNext()) {
				RegisteredDeviceType current = iter.next();
				JSONObject obj = new JSONObject(current);
				jsonDevices.put(obj);
				String deviceLogo = obj.getString("deviceType").split(":")[0].toLowerCase();
				
				writer.write("<tr><td><img src=\"/images/" + deviceLogo + ".jpg\"/></td><td>" 
						+ obj.getString("deviceType") + "</td><td>"
						+ obj.getString("time") + "</td><td>" 
						+ obj.getString("deviceRegistrationKey") + "</td></tr>");
			}
			writer.write("</table>");
		} catch (Exception e) {
			response.setStatus(500);
			log.log(Level.WARNING, "", e);
			
		}
	}
}

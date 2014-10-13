package fr.eurecom.pushing;

import java.io.IOException;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.google.android.gcm.server.Message;
import com.google.android.gcm.server.MulticastResult;
import com.google.android.gcm.server.Sender;

import fr.eurecom.senml.entity.RegisteredDeviceType;
import fr.eurecom.senml.entity.SensorAdmin;
import fr.eurecom.senml.persistence.JDOStorage;

public class GCMPushingServer implements IPushingServer {

	private List<String> REG_IDS, ADMIN_REG_IDS;
	// HARD-CODED, move on a different configuration file!
	private String API_KEY = "AIzaSyDO3RS9FZPTp0d2BNn-0zKUuiiOI45-ED8";
	private boolean initialized = false;

	private static final GCMPushingServer instance = new GCMPushingServer();

	protected GCMPushingServer() {

	}

	public static GCMPushingServer getInstance() {
		if (!instance.isInitialized()) {
			instance.initialize();
		}
		return instance;
	}

	public boolean isInitialized() {
		return initialized;
	}

	public void initialize() {
		REG_IDS = ADMIN_REG_IDS = new ArrayList<String>();
		initialized = true;
	}

	public void refreshDevicesList() {
		REG_IDS = new ArrayList<String>();
		List<RegisteredDeviceType> list = JDOStorage.getInstance().getAll(
				RegisteredDeviceType.class);
		Iterator<RegisteredDeviceType> iter = list.iterator();
		while (iter.hasNext()) {
			RegisteredDeviceType current = iter.next();
			if (isAndroidDevice(current)) {
				if (current.isIsadmin()) {
					if (ADMIN_REG_IDS.indexOf(current) == -1) {
						System.out.println("Refreshing the list of admins...adding "
								+ current.getDeviceRegistrationKey());
						ADMIN_REG_IDS.add(current.getDeviceRegistrationKey());
					}
				} else {
					if (REG_IDS.indexOf(current) == -1) {
						System.out.println("Refreshing the list...adding "
								+ current.getDeviceRegistrationKey());
						REG_IDS.add(current.getDeviceRegistrationKey());
					}
				}
			}
		}
	}

	/**
	 * Return true if the device is of type Android
	 * 
	 * @param device
	 * @return
	 */
	private boolean isAndroidDevice(RegisteredDeviceType device) {
		System.out.print("isAndroidDevice? ");
		if (device.getDeviceType().toLowerCase().indexOf("android") > -1) {
			System.out.println("Yes");
		} else {
			System.out.println("No");
		}
		return (device.getDeviceType().toLowerCase().indexOf("android") > -1);
	}

	@Override
	public void sendNotification(String data) throws JSONException {
		// Refresh the devices list and put it on the JSON object
		refreshDevicesList();
		System.out.println("Calling gcmnotify with data = " + data);
		GCMnotify(data);
		// TODO: Check that is part of monitored sensors
		GCMnotify(data, true);
	}

	private void GCMnotify(String data) {
		
		System.out.println("Sending data: " + data);

		// Instance of com.android.gcm.server.Sender, that does the
		// transmission of a Message to the Google Cloud Messaging service.
		Sender sender = new Sender(API_KEY);

		// This Message object will h	old the data that is being transmitted
		// to the Android client devices. For this demo, it is a simple text
		// string, but could certainly be a JSON object.
		Message message = new Message.Builder()

				// If multiple messages are sent using the same .collapseKey()
				// the android target device, if it was offline during earlier
				// message
				// transmissions, will only receive the latest message for that
				// key when
				// it goes back on-line.
				// .collapseKey(collapseKey)
				.timeToLive(30).delayWhileIdle(true)
				.addData("type", "zoneupdated")
				.addData("notificationmessage", "WL-BOX, zone updated: " + data.replace("ZoneUpdated", ""))
				.addData("alertstatus", "info")
				.addData("message", data).build();
		
		if (REG_IDS.size() == 0) {
			// Noone to notify
			return;
		}
		try {
			// use this for multicast messages.  The second parameter
			// of sender.send() will need to be an array of register ids.
			
			MulticastResult result = sender.send(message, REG_IDS, 1);
			
			if (result.getResults() != null) {
				int canonicalRegId = result.getCanonicalIds();
				if (canonicalRegId != 0) {
					
				}
			} else {
				int error = result.getFailure();
				System.out.println("Broadcast failure: " + error);
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	// TODO: Only one constructor, most of the code is the same...
	private void GCMnotify(String data, boolean isadmin) {
		System.out.println("ADMIN! Sending data: " + data);

		// Instance of com.android.gcm.server.Sender, that does the
		// transmission of a Message to the Google Cloud Messaging service.
		Sender sender = new Sender(API_KEY);

		// This Message object will h	old the data that is being transmitted
		// to the Android client devices. For this demo, it is a simple text
		// string, but could certainly be a JSON object.
		Message message = new Message.Builder()

				// If multiple messages are sent using the same .collapseKey()
				// the android target device, if it was offline during earlier
				// message
				// transmissions, will only receive the latest message for that
				// key when
				// it goes back on-line.
				// .collapseKey(collapseKey)
				.timeToLive(30).delayWhileIdle(true)
				.addData("type", "alert")
				.addData("notificationmessage", "WL-BOX admin, SENSOR ALERT!")
				.addData("alertstatus", "alert")
				.addData("message", data).build();
		
		if (ADMIN_REG_IDS.size() == 0) {
			// Noone to notify
			System.out.println("Admin IDS empty");
			return;
		}
		try {
			// use this for multicast messages.  The second parameter
			// of sender.send() will need to be an array of register ids.
			
			MulticastResult result = sender.send(message, ADMIN_REG_IDS, 1);
			
			if (result.getResults() != null) {
				System.out.println("!) null");
				int canonicalRegId = result.getCanonicalIds();
				if (canonicalRegId != 0) {

				}
			} else {
				int error = result.getFailure();
				System.out.println("Broadcast failure: " + error);
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

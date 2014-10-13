package fr.eurecom.pushing;

import org.json.JSONException;

public interface IPushingServer {
	public void sendNotification(String data) throws JSONException;
}

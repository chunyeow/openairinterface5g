package fr.eurecom.restlet.resources.common;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.json.JSONObject;
import org.restlet.data.Form;
import org.restlet.data.Reference;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.ext.wadl.MethodInfo;
import org.restlet.representation.Representation;
import org.restlet.resource.Get;

import com.google.appengine.api.channel.ChannelServiceFactory;

public class ContactsSubResource extends ContactBaseResource {
	private static final Logger log = Logger.getLogger("ContactSubResource");

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void describePost(MethodInfo mInfo) {
	}



	/**
	 * Handle HTTP GET Subscribe method
	 * 
	 * @param entity - must contains the client id channel. 
	 * @return a JSON Representation of the token. 
	 */
	@Get
	public Representation subscribe(Representation entity) {
		Reference ref = getRequest().getResourceRef();
		Form formQuery = ref.getQueryAsForm();
		JSONObject json = new JSONObject();
		
		try {
			String clientID = formQuery.getFirstValue("clientID", "").trim();
			if (clientID.isEmpty()) {
				log.warning("client ID not found");
				setStatus(Status.CLIENT_ERROR_EXPECTATION_FAILED,
						"Must provide a clientID field.");
				return null;
			}
			String token = getToken(clientID);
			if (token == null) {
				token = ChannelServiceFactory.getChannelService().createChannel(clientID);
				log.info("Create a new token [" + token + "]");
				addSubscriber(clientID, token);
			}
			else {
				log.info("Token already existing for client " + clientID);
			}
			
			json.append("token", token);
			return new JsonRepresentation(json);
			
			
		} catch (Exception e) {
			e.printStackTrace();
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}
}
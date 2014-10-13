package fr.eurecom.restlet.resources.common;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.restlet.data.Status;
import org.restlet.ext.wadl.WadlServerResource;

import com.google.appengine.api.channel.ChannelMessage;
import com.google.appengine.api.channel.ChannelService;
import com.google.appengine.api.channel.ChannelServiceFactory;

import fr.eurecom.restlet.resources.persistence.Storage;

public abstract class ContactBaseResource extends WadlServerResource {
	private final static String CONTACTS_UPDATED = "contactsupdated";
	private static final Logger log = Logger.getLogger("ContactBaseResource");

	public enum MEMBERS {
		CONTACT, ID, FIRSTNAME, LASTNAME, PHONE, MAIL
	}

	protected static final String CONTACTS = "contacts";
	protected static final String CONTACT = "contact";
	protected static final String ID = "id";
	protected static final String FIRST_NAME = "firstname";
	protected static final String LAST_NAME = "lastname";
	protected static final String PHONE = "phone";
	protected static final String MAIL = "mail";

	protected static final String URL = "url";
	protected static final String REQUEST_QUERY_SORT = "sort";
	
// HACK, this class is also used to support subscribers for senml objects. Move the subscribers channel API Stuffs to a common place.	
	protected static final Map<String, Token> subscribers = new HashMap<String, Token>();

	final static class Token {
		public String token;
		public long timestamp;
		
		public Token(String token) {
			this.token = token;
			this.timestamp = System.currentTimeMillis();
		}
	}
	/**
	 * Retrieve the contacts from the persistence layer.
	 * 
	 * @return the contact's list.
	 */
	protected List<Contact> getContacts() {
		// Dispatcher d = (Dispatcher)getApplication();
		return Storage.getInstance().getContacts(MEMBERS.ID, 0, 999);
	}

	/**
	 * Retrieve the contacts sorted by Last name from the persistence layer.
	 * 
	 * @return a sorted contacts list.
	 */
	@SuppressWarnings("unchecked")
	protected List<Contact> getSortedContacts(String sort) {
		try {
			MEMBERS mSorted = sort == null || sort.trim().isEmpty() ? MEMBERS.LASTNAME
					: MEMBERS.valueOf(sort.trim().toUpperCase());

			return Storage.getInstance().getContacts(mSorted, 0, 999);
		} catch (Exception e) {
			e.printStackTrace();
			return Collections.EMPTY_LIST;
		}
	}

	/**
	 * Retrieve the contact which matches this id from the persistence layer.
	 * 
	 * @param id
	 * @return contact or null.
	 */
	protected Contact getContact(int id) {
		return Storage.getInstance().getContact(id);
	}

	/**
	 * Delete a contact from the persistence layer.
	 * 
	 * @param id
	 * @return Status.SUCCESS_NO_CONTENT if deleted,
	 *         Status.CLIENT_ERROR_NOT_FOUND otherwise.
	 */
	protected Status removeContact(int id) {
		if (Storage.getInstance().removeContact(id)) {
			sendUpdateSubscribers(CONTACTS_UPDATED);
			return Status.SUCCESS_NO_CONTENT;
		}
		else {
			return Status.CLIENT_ERROR_NOT_FOUND;
		}
	}

	/**
	 * Save a new contact to the persistence layer. (restricted to 50 contacts
	 * currently).
	 * 
	 * @param contact
	 * @return Status.SUCCESS_CREATED if saved,
	 *         Status.SERVER_ERROR_INSUFFICIENT_STORAGE otherwise.
	 */
	protected Status addContact(Contact contact) {
		if (Storage.getInstance().saveContact(contact)) {
			sendUpdateSubscribers(CONTACTS_UPDATED);
			return Status.SUCCESS_CREATED;
		}
		return Status.SERVER_ERROR_INSUFFICIENT_STORAGE;
	}

	/**
	 * Update a contact to the persistence layer.
	 * 
	 * @param contact
	 * @return Status.SUCCESS_OK if updated, Status.CLIENT_ERROR_NOT_FOUND
	 *         otherwise.
	 */
	protected Status updateContact(Contact contact) {
		if (Storage.getInstance().saveContact(contact)) {
			sendUpdateSubscribers(CONTACTS_UPDATED);
			return Status.SUCCESS_NO_CONTENT;
		} else {
			return Status.CLIENT_ERROR_NOT_FOUND;
		}
	}
	

	public static void addSubscriber(String clientID, String token) {
		subscribers.put(clientID, new Token(token));
	}
	
	public static void deleteSubscriber(String clientID) {
		subscribers.remove(clientID);
	}
	
	public static String getToken(String clientID) {
		Token t = subscribers.get(clientID);
		if (t != null && (System.currentTimeMillis() - t.timestamp) < 7200000) {
			return t.token;
		}
		else {
			return null;
		}
	}

	public static void sendUpdateSubscribers(String message) {
		ChannelService service = ChannelServiceFactory.getChannelService();
		try {
			service.sendMessage(new ChannelMessage("I235", message));
			log.info("Notified I235" + message);
		}
		catch (Exception e) {
			log.log(Level.WARNING, "Error Notifying client I235", e);
			System.out.println("Error notifying client I235");
		}
		
/*		for (String clientID : subscribers.keySet()) {
			System.out.println("Will notify " + clientID + " " + message);
			service.sendMessage(new ChannelMessage(clientID, message));
		}
 */ 	
 	}
}

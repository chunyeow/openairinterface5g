package fr.eurecom.restlet.resources.persistence;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import fr.eurecom.restlet.resources.common.Contact;
import fr.eurecom.restlet.resources.common.ContactBaseResource.MEMBERS;

/**
 * This class is a naive implementation of contact's datastore, it should be
 * replaced by a NOSQL JDO storage.
 */
// TODO: Replace this class by NOSQL JDO implementation.
public class Storage {

	/**
	 * Array of contacts. Restricted to 50 contacts.
	 */
	private final Contact[] backend = new Contact[50];

	/**
	 * Storage instance.
	 */
	private static Storage instance = null;

	private static final String TEMPLATE_SORT = "%s-%s";

	/**
	 * Constructs a storage. Initialize a bunch of contacts.
	 */
	private Storage() {
		backend[0] = new Contact(0, "Bonnie", "Parker", "bonnie@gangsters.org",
				"+1-555-329-4589");
		backend[1] = new Contact(1, "Clyde", "Barrow", "clyde@gangsters.org",
				"+1-555-329-4589");
		backend[2] = new Contact(2, "John", "Dillinger", "john@gangsters.org",
				"+1-555-355-3232");
		backend[3] = new Contact(3, "Herman", "Lamm", "herman@gangsters.org",
				"+1-555-258-2365");
		backend[4] = new Contact(4, "Willie", "Sutton", "will@gangsters.org",
				"+1-555-258-2365");

		for (int i = 5; i < 50; i++) {
			backend[i] = null;
		}
	}

	/**
	 * Retrieve the singleton storage.
	 * 
	 * @return a Storage instance.
	 */
	public static Storage getInstance() {
		if (instance == null) {
			instance = new Storage();
		}
		return instance;
	}

	/**
	 * Retrieve a list of sorted contacts, starting with the given offset.
	 * 
	 * @param sort
	 *            - Sort parameter.
	 * @param minLimit
	 *            - reserved for a future usage
	 * @param maxLimit
	 *            - reserved for a future usage
	 * @return a sorted list of the contacts.
	 */
	@SuppressWarnings("unchecked")
	public List<Contact> getContacts(MEMBERS sort, int minLimit, int maxLimit) {
		switch (sort) {
		case ID:
			return getSortedById();
		case FIRSTNAME:
			return getSortedByFirstName();
		case LASTNAME:
			return getSortedByLastName();
		case PHONE:
			return getSortedByPhone();
		case MAIL:
			return getSortedByEmail();
		}

		return Collections.EMPTY_LIST;
	}

	/**
	 * Save a contact, the Contact's id must be comprise between 0 and 49.
	 * 
	 * @param contact
	 *            to save.
	 * @return true if correctly saved, false otherwise.
	 */
	public boolean saveContact(Contact contact) {
		if (checkIndex(contact.getId())) {
			backend[contact.getId()] = contact;
			return true;
		}
		return false;
	}

	/**
	 * Delete a contact. Same as {@link #removeContact(int)}
	 * {@code
	 *  removeContact(conctact.getId();
	 * }
	 * 
	 * @param contact
	 *            to delete.
	 * @return true if deleted, false otherwise.
	 */
	public boolean removeContact(Contact contact) {
		return removeContact(contact.getId());
	}

	/**
	 * Delete a contact
	 * 
	 * @param identifier
	 *            - Contact's identifier
	 * @return true if deleted, false otherwise
	 * @see Storage.removeContact(Contact)
	 */
	public boolean removeContact(int identifier) {
		if (checkIndex(identifier)) {
			backend[identifier] = null;
			return true;
		}
		return false;
	}

	/**
	 * Retrieve a contact from an identifier.
	 * 
	 * @param identifier
	 *            - Contact's identifier
	 * @return Contact.
	 */
	public Contact getContact(int identifier) {
		if (checkIndex(identifier)) {
			return backend[identifier];
		}
		return null;
	}

	private boolean checkIndex(int index) {
		return index > -1 && index < 50;
	}

	/**
	 * Sort contacts by email field.
	 * 
	 * @return a sorted list of contacts.
	 */
	private List<Contact> getSortedByEmail() {
		Map<String, Contact> sorted = new TreeMap<String, Contact>(
				String.CASE_INSENSITIVE_ORDER);
		for (int i = 0; i < 50; i++) {
			if (backend[i] != null) {
				sorted.put(unicKey(backend[i].getMail(), i), backend[i]);
			}
		}
		return new ArrayList<Contact>(sorted.values());
	}

	/**
	 * Sort contacts by phone number
	 * 
	 * @return a sorted list of contacts.
	 */
	private List<Contact> getSortedByPhone() {
		Map<String, Contact> sorted = new TreeMap<String, Contact>(
				String.CASE_INSENSITIVE_ORDER);
		for (int i = 0; i < 50; i++) {
			if (backend[i] != null) {
				sorted.put(unicKey(toNumeric(backend[i].getPhone()), i),
						backend[i]);
			}
		}
		return new ArrayList<Contact>(sorted.values());
	}

	/**
	 * Sort contacts by last name
	 * 
	 * @return a sorted list of contacts.
	 */
	private List<Contact> getSortedByLastName() {
		Map<String, Contact> sorted = new TreeMap<String, Contact>(
				String.CASE_INSENSITIVE_ORDER);
		for (int i = 0; i < 50; i++) {
			if (backend[i] != null) {
				sorted.put(unicKey(backend[i].getLastName(), i), backend[i]);
			}
		}
		return new ArrayList<Contact>(sorted.values());
	}

	/**
	 * Sort contacts by first name
	 * 
	 * @return a sorted list of contacts.
	 */
	private List<Contact> getSortedByFirstName() {
		Map<String, Contact> sorted = new TreeMap<String, Contact>(
				String.CASE_INSENSITIVE_ORDER);
		for (int i = 0; i < 50; i++) {
			if (backend[i] != null) {
				sorted.put(unicKey(backend[i].getFirstName(), i), backend[i]);
			}
		}
		return new ArrayList<Contact>(sorted.values());
	}

	/**
	 * Sort contacts by identifier.
	 * 
	 * @return a sorted list of contacts.
	 */
	private List<Contact> getSortedById() {
		return Arrays.asList(backend);
	}

	private static String toNumeric(String phone) {
		char[] chars = phone.toCharArray();
		StringBuilder builder = new StringBuilder();
		for (char c : chars) {
			if (Character.isDigit(c)) {
				builder.append(c);
			}
		}
		return builder.toString();
	}

	/**
	 * Format a unic key (necessary when at least 2 contacts have a same
	 * property value like firstname/lastname..)
	 * 
	 * @param key
	 * @param id
	 * @return
	 */
	private static String unicKey(String key, int id) {
		return String.format(TEMPLATE_SORT, key, id);
	}
}

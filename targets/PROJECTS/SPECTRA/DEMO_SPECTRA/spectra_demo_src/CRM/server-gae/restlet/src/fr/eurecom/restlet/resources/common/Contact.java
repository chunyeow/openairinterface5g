package fr.eurecom.restlet.resources.common;

/**
 * This class implements the Contact entity.
 * 
 */
public class Contact {

	// private static final long serialVersionUID = 7390103290165670089L;
	/**
	 * Contact's identifier.
	 */
	private final int id;
	/**
	 * Contact's first name
	 */
	private String firstName = "N/A";
	/**
	 * Contact's last name
	 */
	private String lastName = "N/A";
	/**
	 * Contact's email address
	 */
	private String mail = "N/A";
	/**
	 * Contact's phone number
	 */
	private String phone = "N/A";

	/**
	 * Constructs a contact with the specified id.
	 * 
	 * @param id
	 */
	public Contact(int id) {
		this.id = id;
	}

	/**
	 * Constructs a contact with the specified values.
	 * 
	 * @param id
	 *            Contact's identifier.
	 * @param firstname
	 *            Contact's first name.
	 * @param lastname
	 *            Contact's last name.
	 * @param mail
	 *            Contact's email address.
	 * @param phone
	 *            Contact's Phone number.
	 */
	public Contact(int id, String firstname, String lastname, String mail,
			String phone) {
		this.id = id;
		this.firstName = firstname;
		this.lastName = lastname;
		this.mail = mail;
		this.phone = phone;
	}

	/**
	 * 
	 * @return the contact id.
	 */
	public int getId() {
		return id;
	}

	/**
	 * Set an email address
	 * 
	 * @param mail
	 *            email address.
	 */
	public void setMail(String mail) {
		this.mail = mail;
	}

	/**
	 * Retrieve the email address.
	 * 
	 * @return the mail address if set, "N/A" otherwise.
	 */
	public String getMail() {
		return mail;
	}

	/**
	 * Retrieve the first name.
	 * 
	 * @return the first name if set, "N/A" otherwise.
	 */
	public String getFirstName() {
		return firstName;
	}

	/**
	 * Set a first name.
	 * 
	 * @param firstname
	 *            to set.
	 */
	public void setFirstName(String firstname) {
		this.firstName = firstname;
	}

	/**
	 * Retrieve the last name.
	 * 
	 * @return the last name set, "N/A" otherwise.
	 */
	public String getLastName() {
		return lastName;
	}

	/**
	 * Set a last name.
	 * 
	 * @param lastname
	 *            to set, "N/A" otherwise.
	 */
	public void setLastName(String lastname) {
		this.lastName = lastname;
	}

	/**
	 * Retrieve the phone number.
	 * 
	 * @return the phone set, "N/A" otherwise.
	 */
	public String getPhone() {
		return phone;
	}

	/**
	 * Set the phone number.
	 * 
	 * @param phone
	 *            number to set.
	 */
	public void setPhone(String phone) {
		this.phone = phone;
	}
}

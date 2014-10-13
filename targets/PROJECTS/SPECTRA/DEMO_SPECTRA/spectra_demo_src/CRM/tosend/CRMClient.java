package fr.eurecom.spectra.crrm.client;

import java.io.IOException;

import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import org.restlet.Context;
import org.restlet.data.MediaType;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.representation.Representation;
import org.restlet.resource.ClientResource;
import org.restlet.resource.ResourceException;

import com.google.gson.Gson;

import fr.eurecom.spectra.crrm.entities.Decision;
import fr.eurecom.spectra.crrm.entities.Measurement;
import fr.eurecom.spectra.crrm.entities.Policy;

public class CRMClient {

	private static final String BASE_URL = "http://1.gae-spectra.appspot.com/";

	// http://localhost:8888/
	public static void main(String[] args) throws IOException,
			ResourceException {

		final Context context = new Context();

		// Define our Restlet client resources policies.
		ClientResource policiesResource = new ClientResource(context,
				BASE_URL + "policies");
		ClientResource policyResource = null;

		// Define our Restlet client resources measurements.
		ClientResource measurementsResource = new ClientResource(
				context, BASE_URL + "measurements");
		ClientResource measurementResource = null;

		// Define our Restlet client resources decisions.
		ClientResource decisionsResource = new ClientResource(
				context, BASE_URL + "decisions");
		ClientResource decisionResource = null;

		context.getParameters().set("maxConnectionsPerHost", "20");

		// Create a new Policy
		Policy p = new Policy("p1", "priority.",
				"Priority of the user on the secondary user", 1);

		Gson gson = new Gson();
		String myp = gson.toJson(p);

		JSONObject jObject = (JSONObject) JSONSerializer.toJSON(myp);
		Representation entity = new JsonRepresentation(jObject);
		entity.setMediaType(MediaType.APPLICATION_JSON);
		policiesResource.setRequestEntityBuffering(true);
		Representation reply = policiesResource.post(entity,
				MediaType.APPLICATION_JSON);
		policiesResource.release();
		// Create a new measurement
		Measurement m1 = new Measurement("m1", "SNIR",
				"the signal noise interference ratio", 1,
				"Physical measurement");

		Gson gson2 = new Gson();
		String mym = gson2.toJson(m1);
		JSONObject jObject2 = (JSONObject) JSONSerializer.toJSON(mym);
		Representation entity2 = new JsonRepresentation(jObject2);
		entity2.setMediaType(MediaType.APPLICATION_JSON);
		measurementsResource.setRequestEntityBuffering(true);
		Representation reply2 = measurementsResource.post(entity2,
				MediaType.APPLICATION_JSON);

		// Create a new Decision
		Decision d1 = new Decision("md1", "Band",
				"the allocated band for the UE", 2, 10);
		Gson gson3 = new Gson();
		String myd = gson3.toJson(d1);
		JSONObject jObject3 = (JSONObject) JSONSerializer.toJSON(myd);
		Representation entity3 = new JsonRepresentation(jObject3);
		entity2.setMediaType(MediaType.APPLICATION_JSON);
		decisionsResource.setRequestEntityBuffering(true);
		Representation reply3 = decisionsResource.post(entity3,
				MediaType.APPLICATION_JSON);

		if (policiesResource.getStatus().isSuccess()) {
			policyResource = new ClientResource(context,
					reply.getLocationRef() + "/" + p.getpID());

			policiesResource.get(MediaType.APPLICATION_JSON);
			if (policiesResource.getStatus().isSuccess()
					&& policiesResource.getResponseEntity()
							.isAvailable()) {

				Representation rep = policiesResource
						.getResponseEntity();
				Gson gtest = new Gson();
				String text = rep.getText();
				Policy[] ptest = gtest.fromJson(text, Policy[].class);
				System.out.println("Extracted Policy "
						+ ptest[0].getpID());
			}

		}
		// policiesResource.release();
		policiesResource.delete();
		measurementsResource.delete();
		decisionsResource.delete();
	}

	/**
	 * Prints the resource's representation.
	 * 
	 * @param clientResource
	 *            The Restlet client resource.
	 * @throws IOException
	 * @throws ResourceException
	 */
	public static void get(ClientResource clientResource)
			throws IOException, ResourceException {
		clientResource.get(MediaType.APPLICATION_JSON);
		if (clientResource.getStatus().isSuccess()
				&& clientResource.getResponseEntity().isAvailable()) {
			clientResource.get(MediaType.APPLICATION_JSON).write(
					System.out);
			// clientResource.getResponseEntity().write(System.out);
		}
	}

	/**
	 * Returns the Representation of an item.
	 * 
	 * @param item
	 *            the item.
	 * 
	 * @return The Representation of the item.
	 * @throws IOException
	 */
	public static Representation getRepresentation(Policy p)
			throws IOException {

		Gson gson = new Gson();
		String myp = gson.toJson(p);

		JSONObject js = (JSONObject) JSONSerializer.toJSON(myp);
		Representation entity = new JsonRepresentation(js);
		entity.setMediaType(MediaType.APPLICATION_JSON);
		return entity;
	}

}

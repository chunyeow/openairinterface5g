package fr.eurecom.restlet.resources.gae;

import java.util.logging.Logger;

import org.restlet.Request;
import org.restlet.Response;
import org.restlet.Restlet;
import org.restlet.ext.wadl.ApplicationInfo;
import org.restlet.ext.wadl.DocumentationInfo;
import org.restlet.ext.wadl.WadlApplication;
import org.restlet.resource.Directory;
import org.restlet.routing.Router;

import fr.eurecom.restlet.resources.common.SensorPost;
import fr.eurecom.restlet.resources.common.ZonesResource;
import fr.eurecom.restlet.resources.common.ZoneResource;
import fr.eurecom.restlet.resources.common.SensorInResource;


public class DispatcherSenml extends WadlApplication {
	private static final Logger log = Logger.getLogger("DispatcherSenml");

	@Override
	public Restlet createInboundRoot() {
		log.info("SenML dispatcher attaches resources");
		Router router = new Router(getContext());
		router.attachDefault(new Directory(getContext(), "war:///"));
//		web xml mapping = /*
		
		router.attach("/zones", ZonesResource.class);
		router.attach("/zones/{id}", ZoneResource.class);
		router.attach("/sensorsin", SensorInResource.class);
		router.attach("/posttest", SensorPost.class);
		return router;
	}

	@Override
	public ApplicationInfo getApplicationInfo(Request request, Response response) {
		ApplicationInfo info = super.getApplicationInfo(request, response);

		DocumentationInfo docInfo = new DocumentationInfo(
				"emulator-box-services application documentation");
		docInfo.setTitle("Emulator Box Services application infos.");
		info.setDocumentation(docInfo);

		return info;
	}
}

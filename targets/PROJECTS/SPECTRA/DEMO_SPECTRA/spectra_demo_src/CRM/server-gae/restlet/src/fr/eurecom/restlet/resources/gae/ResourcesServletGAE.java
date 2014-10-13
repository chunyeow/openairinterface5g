package fr.eurecom.restlet.resources.gae;

import java.io.IOException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * This class serves for test only. Can be deleted.
 * @author ouest
 *
 */

@SuppressWarnings("serial")
public class ResourcesServletGAE extends HttpServlet {

	@Override
	public void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
		resp.setContentType("text/plain");
		resp.getWriter().println("Hello, world");
	}
	
}

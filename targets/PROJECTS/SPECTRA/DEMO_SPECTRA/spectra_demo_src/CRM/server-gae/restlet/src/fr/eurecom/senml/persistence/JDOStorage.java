package fr.eurecom.senml.persistence;

import java.util.Collection;
import java.util.Collections;
import java.util.List;

import javax.jdo.PersistenceManager;
import javax.jdo.Query;

import fr.eurecom.restlet.resources.persistence.PMF;
import fr.eurecom.senml.entity.Zone;
import fr.eurecom.senml.entity.ZoneAdmin;

/**
 * This class is a jdo wrapper. Implementing the persistence. 
 * @author dalmassi
 *
 */
public class JDOStorage {
	private static final JDOStorage instance = new JDOStorage();
	
	protected JDOStorage() {
		
	}
	
	public static JDOStorage getInstance() {
		return instance;
	}
	
	@SuppressWarnings("unchecked")
/**
 * Wrapper JDO PM.getById, return a detached object. 	
 * @param key The String key 
 * @param c Class of the object
 * @return a typed object of the class.  
 */
	public <T> T getById(String key, Class<T> c) {
		PersistenceManager pm = PMF.get().getPersistenceManager();
		Object attached, detached = null;
		
		try {
			attached = pm.getObjectById(c, key);
			detached = pm.detachCopy(attached);
		}
		finally {
			close(pm);
		}
		return (T) detached;
	}
	
	public <T> void refresh(String key, Class<T> c) {
		PersistenceManager pm = getPM();
		
		try {
			
			Object attached = pm.getObjectById(c, key);
			pm.refresh(attached);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	Query q = null;
	
	@SuppressWarnings("unchecked")
	public <T> List<T> getAll(Class<?> c) {
		List<T> detached = Collections.EMPTY_LIST;
		PersistenceManager pm = getPM();
		Query q = pm.newQuery(c);
		
		try {
			List<T> results = (List<T>) q.execute();
			if (!results.isEmpty()) {
				detached = (List<T>) pm.detachCopyAll(results);
				}
			close(q);
			close(pm);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		return detached;
	}

/**
 * Save an object to dataStore.	
 * @param z object to save
 * @return true if ok, false otherwise. 
 */
	public <T> boolean write(T z) {
		PersistenceManager pm = getPM();
		try {
			pm.makePersistent(z);
//			System.out.println("write jdo done! zone name|" + z.getName() + "| key |" + z.getKey() + "|");
			return true;
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		finally {
		close(pm);
		}
	}
	
	public <T> void delete(T z) {
		PersistenceManager pm = getPM();
		try {
			pm.deletePersistent(z);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		finally {
			close(pm);
		}
	}
	
	public <T> void delete(Collection<T> z) {
		PersistenceManager pm = getPM();
		try {
			pm.deletePersistentAll(z);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		finally {
			close(pm);
		}
	}
	
	public long deleteAll(Class<?> c) {
		PersistenceManager pm = getPM();
		long count = -1;
		try {
			Query q = pm.newQuery(c);
			count = q.deletePersistentAll(); 
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		finally {
			close(q);
			close(pm);
		}
		return count;
	}
	
	
	protected PersistenceManager getPM() {
		return PMF.get().getPersistenceManager();
	}
	
	protected void close(PersistenceManager pm) {
		if (pm != null) {
			pm.close();
		}
	}
	
	protected void close(Query q) {
		if (q != null) {
			q.closeAll();
		}
	}

}

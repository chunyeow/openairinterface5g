package fr.eurecom.senml.persistence;

import java.util.Collections;
import java.util.List;

import javax.jdo.PersistenceManager;
import javax.jdo.Query;
import fr.eurecom.senml.entity.Measure;
import fr.eurecom.senml.entity.Sensor;
import fr.eurecom.senml.entity.SensorAdmin;
import fr.eurecom.senml.entity.Zone;
import fr.eurecom.senml.entity.ZoneAdmin;



public class JDOStorageZone extends JDOStorage {
	private static final JDOStorageZone instance = new JDOStorageZone();
	private Query Q_SUBZONE_ADMIN = null;
	private Query Q_SENSOR_ADMIN = null;
	private Query Q_MEASURE_ADMIN = null;
	
	
	private JDOStorageZone() {
		super();
		createQuerySubZone();
		createQueryZoneSensors();
		createQueryMeasures();
	}
	

	public static JDOStorageZone getInstance() {
		return instance;
	}
	

	@SuppressWarnings("unchecked")
	public <T> List<T> getSubZones(Zone zone) {
		List<T> detached = Collections.EMPTY_LIST;
		try {
			List<T> results = (List<T>) Q_SUBZONE_ADMIN.execute(zone.getKey());
			if (!results.isEmpty()) {
				detached = (List<T>) Q_SUBZONE_ADMIN.getPersistenceManager().detachCopyAll(results);
				}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		return detached;
	}

	@SuppressWarnings("unchecked")
	public <T> List<T> getSensors(Zone zone) {
		List<T> detached = Collections.EMPTY_LIST;
		try {
			List<T> results = (List<T>) Q_SENSOR_ADMIN.execute(zone.getKey());
			if (!results.isEmpty()) {
				detached = (List<T>) Q_SENSOR_ADMIN.getPersistenceManager().detachCopyAll(results);
			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		return detached;
	}
	
	@SuppressWarnings("unchecked")	
	public List<Measure> getMeasures(Sensor sensor) {
		List<Measure> detached = Collections.EMPTY_LIST;
		try {
			List<Measure> results = (List<Measure>) Q_MEASURE_ADMIN.execute(sensor.getUUID());
			if (!results.isEmpty()) {
				detached = (List<Measure>) Q_MEASURE_ADMIN.getPersistenceManager().detachCopyAll(results);
			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	return detached;	
	}
	

	private void createQuerySubZone() {
		if (Q_SUBZONE_ADMIN == null) {
			Q_SUBZONE_ADMIN = getPM().newQuery(ZoneAdmin.class);
			Q_SUBZONE_ADMIN.setFilter("parentKey == keyParentParam");
//			Q_SUBZONE.declareParameters(Key.class.getName() + " keyParentParam");
			Q_SUBZONE_ADMIN.declareParameters("String keyParentParam");
			Q_SUBZONE_ADMIN.compile();
		}
	}
	
	private void createQueryZoneSensors() {
		if (Q_SENSOR_ADMIN == null) {
			PersistenceManager pm = getPM();
//			pm.getFetchPlan().setDetachmentOptions(	FetchPlan.DETACH_LOAD_FIELDS | FetchPlan.DETACH_UNLOAD_FIELDS);
//			pm.getFetchPlan().setMaxFetchDepth(4);
			
			Q_SENSOR_ADMIN = pm.newQuery(SensorAdmin.class);
			Q_SENSOR_ADMIN.setFilter("zoneKey == zoneKeyParam");
			Q_SENSOR_ADMIN.declareParameters("String zoneKeyParam");
			Q_SENSOR_ADMIN.compile();			
		}
	}
	
	private void createQueryMeasures() {
		if (Q_MEASURE_ADMIN == null) {
			PersistenceManager pm = getPM();
			Q_MEASURE_ADMIN = pm.newQuery(Measure.class);
			Q_MEASURE_ADMIN.setFilter("sensorKey == sensorKeyParam");
			Q_MEASURE_ADMIN.declareParameters("String sensorKeyParam");
			Q_MEASURE_ADMIN.compile();
			Q_MEASURE_ADMIN.setIgnoreCache(true);
		}
	}
}

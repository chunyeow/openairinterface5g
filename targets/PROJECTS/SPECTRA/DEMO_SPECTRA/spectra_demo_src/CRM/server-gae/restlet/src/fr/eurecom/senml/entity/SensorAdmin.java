package fr.eurecom.senml.entity;

import java.util.UUID;

import javax.jdo.annotations.Inheritance;
import javax.jdo.annotations.NotPersistent;
import javax.jdo.annotations.PersistenceCapable;
import javax.jdo.annotations.Persistent;

import org.json.JSONException;

import fr.eurecom.pushing.GCMPushingServer;
import fr.eurecom.senml.entity.Measure.ParamTypeValue;
import fr.eurecom.senml.persistence.JDOStorage;

@PersistenceCapable
@Inheritance(customStrategy = "complete-table")
public class SensorAdmin extends Sensor implements ISensorAdmin {

	@NotPersistent
	private final static String ZONE_UPDATED = "ZoneUpdated";

	@Persistent
	boolean active = true;

	public SensorAdmin(String title, IZone zone, String baseUnit, String typeSensor) {
		super(UUID.randomUUID().toString(), title, zone != null ? zone.getKey()
				: null, baseUnit, typeSensor);
		JDOStorage.getInstance().write(this);
	}

	public SensorAdmin(String bn, String title, IZone zone) {
		super(bn, title, zone != null ? zone.getKey() : null);
		JDOStorage.getInstance().write(this);
	}

	@Override
	public void setTitle(String title) {
		this.title = title;
		JDOStorage.getInstance().write(this);
	}

	@Override
	public void destroy() {
		try {
			JDOStorage.getInstance().delete(getMeasures());
		} catch (Exception e) {
			e.printStackTrace();
		}

		JDOStorage.getInstance().delete(this);
	}

	@Override
	public boolean linkZone(IZone zone) {
		return zone == null ? false : linkZone(zone.getKey());
	}

	@Override
	public boolean linkZone(String zoneKey) {
		if (active && checkKey(zoneKey)) {
			this.zoneKey = zoneKey;
			JDOStorage.getInstance().write(this);
			return active;
		}
		return false;
	}

	@Override
	public void unlinkZone() {
		if (zoneKey != null) {
			this.zoneKey = null;
			JDOStorage.getInstance().write(this);
		}
	}

	@Override
	public void deleteMeasure(String measureKey) {
		if (measureKey != null) {
			JDOStorage.getInstance()
					.delete(JDOStorage.getInstance().getById(measureKey,
							Measure.class));
		}
		fr.eurecom.restlet.resources.common.ContactBaseResource
				.sendUpdateSubscribers(ZONE_UPDATED + getZone().getName());
	}

	@Override
	public void deleteMeasure(Measure measure) {
		JDOStorage.getInstance().delete(measure);
		fr.eurecom.restlet.resources.common.ContactBaseResource
				.sendUpdateSubscribers(ZONE_UPDATED + getZone().getName());
	}

	private boolean checkKey(String key) {
		return key != null && !key.trim().isEmpty();
	}

	@Override
	public Measure addMeasure(String measureName, Units measureUnit,
			ParamTypeValue measureTypeValue, String measureValue,
			long timeMeasure, String type) {

		Measure m = new Measure(getUUID(), measureName, measureUnit,
				measureTypeValue, measureValue, timeMeasure, type);
		JDOStorage.getInstance().write(m);

		// fr.eurecom.restlet.resources.common.ContactBaseResource.sendUpdateSubscribers(ZONE_UPDATED
		// + getZone().getName());
		GCMPushingServer GCM = GCMPushingServer.getInstance();
		try {
			GCM.sendNotification(ZONE_UPDATED + getZone().getName());
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return m;
	}
	
	@Override
	public Measure addMeasure(String measureName, Units measureUnit,
			ParamTypeValue measureTypeValue, String measureValue,
			long timeMeasure, String type, String sum) {

		Measure m = new Measure(getUUID(), measureName, measureUnit,
				measureTypeValue, measureValue, timeMeasure, type, sum);
		JDOStorage.getInstance().write(m);

		// fr.eurecom.restlet.resources.common.ContactBaseResource.sendUpdateSubscribers(ZONE_UPDATED
		// + getZone().getName());
		GCMPushingServer GCM = GCMPushingServer.getInstance();
		try {
			GCM.sendNotification(ZONE_UPDATED + getZone().getName());
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return m;
	}

}

package org.atriasoft.gale.resource;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotSame;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.context.GaleContext;
import org.atriasoft.gale.context.GaleContextTest;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

/**
 * Life cycle of the resources, without OpenGL: the test thread has no OpenGL
 * context, so every deletion waits for {@link ResourceManager#updateContext()}.
 */
class ResourceLifecycleTest {

	/** A resource that counts its clean-ups and its uploads, and touches no OpenGL. */
	private static final class CountingResource extends Resource {
		private int cleanUps = 0;
		private int updates = 0;

		CountingResource(final String name) {
			super(name);
		}

		@Override
		public void cleanUp() {
			this.cleanUps++;
		}

		@Override
		public boolean updateContext() {
			this.updates++;
			return true;
		}
	}

	private ResourceManager manager;

	@BeforeEach
	void setUp() {
		new GaleContextTest();
		this.manager = GaleContext.getContext().getResourcesManager();
		// First update done: the next ones only send what was flushed.
		this.manager.updateContext();
	}

	@Test
	void releaseToZeroLeavesTheManagerAndCleansUpAtTheNextUpdate() {
		final CountingResource resource = new CountingResource("one");
		assertEquals(1, this.manager.getResourceCount());
		resource.release();
		assertTrue(resource.isReleased());
		assertEquals(0, this.manager.getResourceCount());
		assertNull(this.manager.localKeep("one"));
		assertEquals(0, resource.cleanUps, "no OpenGL context: the clean-up waits for the OpenGL thread");
		this.manager.updateContext();
		assertEquals(1, resource.cleanUps);
		this.manager.updateContext();
		assertEquals(1, resource.cleanUps);
	}

	@Test
	void aKeptResourceLivesUntilItsLastRelease() {
		final CountingResource resource = new CountingResource("shared");
		resource.keep();
		assertEquals(2, resource.getCount());
		resource.release();
		assertFalse(resource.isReleased());
		assertSame(resource, this.manager.localKeep("shared"));
		resource.release();
		assertTrue(resource.isReleased());
		this.manager.updateContext();
		assertEquals(1, resource.cleanUps);
	}

	@Test
	void aSecondReleaseIsIgnored() {
		final CountingResource resource = new CountingResource("twice");
		resource.release();
		resource.release();
		assertEquals(0, resource.getCount());
		this.manager.updateContext();
		assertEquals(1, resource.cleanUps);
	}

	@Test
	void aResourceIsRegisteredOnce() {
		final CountingResource resource = new CountingResource("once");
		this.manager.localAdd(resource);
		this.manager.localAdd(resource);
		assertEquals(1, this.manager.getResourceCount());
		resource.release();
		assertEquals(0, this.manager.getResourceCount());
		// A released resource does not come back.
		this.manager.localAdd(resource);
		assertEquals(0, this.manager.getResourceCount());
	}

	@Test
	void aReleasedResourceIsNotUpdated() {
		final CountingResource queued = new CountingResource("queued");
		this.manager.update(queued);
		queued.release();
		this.manager.update(queued);
		this.manager.updateContext();
		assertEquals(0, queued.updates);
		final CountingResource living = new CountingResource("living");
		this.manager.update(living);
		this.manager.updateContext();
		assertEquals(1, living.updates);
	}

	@Test
	void aWorkForTheOpenGlThreadWaitsForTheNextUpdate() {
		final AtomicInteger runs = new AtomicInteger();
		this.manager.runOnGlThread(runs::incrementAndGet);
		assertEquals(0, runs.get());
		this.manager.updateContext();
		assertEquals(1, runs.get());
		this.manager.updateContext();
		assertEquals(1, runs.get());
	}

	@Test
	void aCachedTextureIsKeptOnEachHit() {
		final ResourceTexture2 first = ResourceTexture2.createNamed("texture");
		final ResourceTexture2 second = ResourceTexture2.createNamed("texture");
		assertSame(first, second);
		assertEquals(2, first.getCount());
		assertEquals(1, this.manager.getResourceCount());
		first.release();
		assertFalse(first.isReleased(), "the other user still has it");
		second.release();
		assertTrue(first.isReleased());
		final ResourceTexture2 again = ResourceTexture2.createNamed("texture");
		assertNotSame(first, again);
		assertEquals(1, again.getCount());
	}

	@Test
	void aCachedResourceOfAnotherTypeThrows() {
		final CountingResource resource = new CountingResource("taken");
		assertThrows(IllegalStateException.class, () -> ResourceTexture2.createNamed("taken"));
		assertEquals(1, resource.getCount());
		assertEquals(1, this.manager.getResourceCount());
	}

	@Test
	void aVertexArrayIsRegisteredOnceAndForgottenWhenReleased() {
		final ResourceVirtualArrayObject vao = ResourceVirtualArrayObject.create(new float[] { 0, 0, 0, 1, 0, 0, 0, 1, 0 },
				new float[] { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, new int[] { 0, 1, 2 });
		final ResourceVirtualArrayObject dynamic = ResourceVirtualArrayObject.createDynamic();
		assertEquals(2, this.manager.getResourceCount());
		vao.flush();
		vao.release();
		dynamic.release();
		assertEquals(0, this.manager.getResourceCount());
		// Never sent to the graphic card: nothing to delete, and a released array is not sent any more.
		this.manager.updateContext();
		assertTrue(vao.updateContext());
		assertEquals(-1, vao.getGLID());
	}

	@Test
	void releasingAProgramReleasesItsShaders() {
		final Uri vertex = new Uri("DATA", "test.vert");
		final Uri fragment = new Uri("DATA", "test.frag");
		final ResourceProgram program = ResourceProgram.create(vertex, fragment);
		assertSame(program, ResourceProgram.create(vertex, fragment));
		assertEquals(2, program.getCount());
		// Looked up without keeping it: the program is its only user.
		final Resource shader = this.manager.localKeep(vertex.getValue());
		assertEquals(1, shader.getCount());
		assertEquals(3, this.manager.getResourceCount());
		program.release();
		assertFalse(program.isReleased());
		program.release();
		// The clean-up of the program releases the shaders, deleted in the same update.
		this.manager.updateContext();
		assertTrue(shader.isReleased());
		assertEquals(0, this.manager.getResourceCount());
	}

	@Test
	void releasingAColoredObjectGivesBackItsProgram() {
		final ResourceColored3DObject object = ResourceColored3DObject.create();
		// The object, its program and the two shaders of the program.
		assertEquals(4, this.manager.getResourceCount());
		object.release();
		this.manager.updateContext();
		assertEquals(0, this.manager.getResourceCount());
	}
}

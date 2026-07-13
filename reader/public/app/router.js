/**
 * View router — manages tab switching and renders the active view.
 * Each view module is loaded lazily on first activation.
 */

import { state, dispatch } from './state.js';

const VIEW_TABS = {
  rail:  { label: '🗄️ Rail View',    id: 'tab-rail'  },
  home:  { label: '🏠 Home View',    id: 'tab-home'  },
  mat:   { label: '📋 Materiaallijst', id: 'tab-mat'   },
  units: { label: '📊 Units',        id: 'tab-units' },
};

// Lazy-loaded view modules
const _views = {};

export async function switchView(viewName) {
  if (!VIEW_TABS[viewName]) return;

  // Update tab UI
  for (const [key, { id }] of Object.entries(VIEW_TABS)) {
    document.getElementById(id)?.classList.toggle('active', key === viewName);
  }

  // Show/hide view panels
  document.getElementById('rail-view')?.style.setProperty('display', viewName === 'rail'  ? 'flex' : 'none');
  document.getElementById('home-view')?.style.setProperty('display', viewName === 'home'  ? 'flex' : 'none');
  document.getElementById('mat-view')?.style.setProperty('display', viewName === 'mat'   ? 'flex' : 'none');
  document.getElementById('units-view')?.style.setProperty('display', viewName === 'units' ? 'flex' : 'none');

  // Show/hide sidebars
  document.getElementById('sidebar-rail')?.style.setProperty('display', viewName === 'rail' ? 'flex' : 'none');
  document.getElementById('sidebar-home')?.style.setProperty('display', viewName === 'home' ? 'flex' : 'none');
  document.getElementById('sidebar-mat')?.style.setProperty('display', viewName === 'mat'  ? 'flex' : 'none');
  // units view has no sidebar

  dispatch({ type: 'SET_VIEW', view: viewName });

  // Load and mount view module
  if (!_views[viewName]) {
    try {
      const mod = await import(`../components/${viewName}-view.js`);
      _views[viewName] = mod;
      // Mount the view if it has a mount function
      if (mod.mount) {
        const container = document.getElementById(`${viewName}-view`);
        if (container) mod.mount(container);
      }
    } catch (e) {
      console.warn(`[router] View component ${viewName}-view.js not found`);
    }
  }
  _views[viewName]?.activate?.();
}

/** Wire tab click handlers (called after DOM is ready). */
export function initRouter() {
  for (const [viewName, { id }] of Object.entries(VIEW_TABS)) {
    document.getElementById(id)?.addEventListener('click', () => switchView(viewName));
  }

  // Initial view from state
  const currentView = state.get().activeView;
  switchView(currentView);
}

/**
 * View router — manages tab switching and renders the active view.
 * Each view module is loaded lazily on first activation.
 */

import { state, dispatch } from './state.js';

const VIEW_TABS = {
  rail: { label: '🗄️ Rail View',    id: 'tab-rail' },
  home: { label: '🏠 Home View',    id: 'tab-home' },
  mat:  { label: '📋 Materiaallijst', id: 'tab-mat'  },
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
  document.getElementById('rail-view')?.style.setProperty('display', viewName === 'rail' ? 'flex' : 'none');
  document.getElementById('home-view')?.style.setProperty('display', viewName === 'home' ? 'flex' : 'none');
  document.getElementById('mat-view')?.style.setProperty('display', viewName === 'mat'  ? 'flex' : 'none');

  // Show/hide sidebars
  document.getElementById('sidebar-rail')?.style.setProperty('display', viewName === 'rail' ? 'flex' : 'none');
  document.getElementById('sidebar-home')?.style.setProperty('display', viewName === 'home' ? 'flex' : 'none');
  document.getElementById('sidebar-mat')?.style.setProperty('display', viewName === 'mat'  ? 'flex' : 'none');

  dispatch({ type: 'SET_VIEW', view: viewName });

  // Load and activate view module
  if (!_views[viewName]) {
    try {
      _views[viewName] = await import(`../components/${viewName}-view.js`);
    } catch {
      // View component not yet implemented — ignore
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

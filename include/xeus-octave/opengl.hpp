////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016-2020 The Octave Project Developers
//
// See the file COPYRIGHT.md in the top-level directory of this
// distribution or <https://octave.org/copyright/>.
//
// This file is part of Octave.
//
// Octave is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Octave is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Octave; see the file COPYING.  If not, see
// <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////

#ifndef XEUS_OCTAVE_OPENGL_H
#define XEUS_OCTAVE_OPENGL_H

#include <glad/glad.h>

namespace octave
{
class opengl_functions
{
public:

  opengl_functions(void) {}

  opengl_functions(opengl_functions const&) = default;

  opengl_functions& operator=(opengl_functions const&) = default;

  virtual ~opengl_functions(void) = default;

  // If OpenGL is not available, opengl_functions will be a dummy
  // class that does nothing.  This makes the implementation of
  // other things that rely on this class slightly simpler.

  virtual void glAlphaFunc(GLenum func, GLclampf ref) { ::glAlphaFunc(func, ref); }

  virtual void glBegin(GLenum mode) { ::glBegin(mode); }

  virtual void glBindTexture(GLenum target, GLuint texture) { ::glBindTexture(target, texture); }

  virtual void glBitmap(
    GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, GLubyte const* bitmap
  )
  {
    ::glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
  }

  virtual void glBlendFunc(GLenum sfactor, GLenum dfactor) { ::glBlendFunc(sfactor, dfactor); }

  virtual void glCallList(GLuint list) { ::glCallList(list); }

  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
  {
    ::glClearColor(red, green, blue, alpha);
  }

  virtual void glClear(GLbitfield mask) { ::glClear(mask); }

  virtual void glClipPlane(GLenum plane, GLdouble const* equation) { ::glClipPlane(plane, equation); }

  virtual void glColor3dv(GLdouble const* v) { ::glColor3dv(v); }

  virtual void glColor3f(GLfloat red, GLfloat green, GLfloat blue) { ::glColor3f(red, green, blue); }

  virtual void glColor3fv(GLfloat const* v) { ::glColor3fv(v); }

  virtual void glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
  {
    ::glColor4d(red, green, blue, alpha);
  }

  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
  {
    ::glColor4f(red, green, blue, alpha);
  }

  virtual void glColor4fv(GLfloat const* v) { ::glColor4fv(v); }

  virtual void glDeleteLists(GLuint list, GLsizei range) { ::glDeleteLists(list, range); }

  virtual void glDeleteTextures(GLsizei n, GLuint const* textures) { ::glDeleteTextures(n, textures); }

  virtual void glDepthFunc(GLenum func) { ::glDepthFunc(func); }

  virtual void glDisable(GLenum cap) { ::glDisable(cap); }

  virtual void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid const* pixels)
  {
    ::glDrawPixels(width, height, format, type, pixels);
  }

  virtual void glEdgeFlag(GLboolean flag) { ::glEdgeFlag(flag); }

  virtual void glEnable(GLenum cap) { ::glEnable(cap); }

  virtual void glEndList(void) { ::glEndList(); }

  virtual void glEnd(void) { ::glEnd(); }

  virtual void glFinish(void) { ::glFinish(); }

  virtual GLuint glGenLists(GLsizei range) { return ::glGenLists(range); }

  virtual void glGenTextures(GLsizei n, GLuint* textures) { ::glGenTextures(n, textures); }

  virtual void glGetBooleanv(GLenum pname, GLboolean* data) { ::glGetBooleanv(pname, data); }

  virtual void glGetDoublev(GLenum pname, GLdouble* data) { ::glGetDoublev(pname, data); }

  virtual GLenum glGetError(void) { return ::glGetError(); }

  virtual void glGetFloatv(GLenum pname, GLfloat* data) { ::glGetFloatv(pname, data); }

  virtual void glGetIntegerv(GLenum pname, GLint* data) { ::glGetIntegerv(pname, data); }

  virtual GLubyte const* glGetString(GLenum name) { return ::glGetString(name); }

  virtual void glHint(GLenum target, GLenum mode) { ::glHint(target, mode); }

  virtual void glInitNames(void) { ::glInitNames(); }

  virtual GLboolean glIsEnabled(GLenum cap) { return ::glIsEnabled(cap); }

  virtual void glLightfv(GLenum light, GLenum pname, GLfloat const* params) { ::glLightfv(light, pname, params); }

  virtual void glLineStipple(GLint factor, GLushort pattern) { ::glLineStipple(factor, pattern); }

  virtual void glLineWidth(GLfloat width) { ::glLineWidth(width); }

  virtual void glLoadIdentity(void) { ::glLoadIdentity(); }

  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param) { ::glMaterialf(face, pname, param); }

  virtual void glMaterialfv(GLenum face, GLenum pname, GLfloat const* params) { ::glMaterialfv(face, pname, params); }

  virtual void glMatrixMode(GLenum mode) { ::glMatrixMode(mode); }

  virtual void glMultMatrixd(GLdouble const* m) { ::glMultMatrixd(m); }

  virtual void glNewList(GLuint list, GLenum mode) { ::glNewList(list, mode); }

  virtual void glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz) { ::glNormal3d(nx, ny, nz); }

  virtual void glNormal3dv(GLdouble const* v) { ::glNormal3dv(v); }

  virtual void
  glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
  {
    ::glOrtho(left, right, bottom, top, near_val, far_val);
  }

  virtual void glPixelStorei(GLenum pname, GLint param) { ::glPixelStorei(pname, param); }

  virtual void glPixelZoom(GLfloat xfactor, GLfloat yfactor) { ::glPixelZoom(xfactor, yfactor); }

  virtual void glPolygonMode(GLenum face, GLenum mode) { ::glPolygonMode(face, mode); }

  virtual void glPolygonOffset(GLfloat factor, GLfloat units) { ::glPolygonOffset(factor, units); }

  virtual void glPopAttrib(void) { ::glPopAttrib(); }

  virtual void glPopMatrix(void) { ::glPopMatrix(); }

  virtual void glPopName(void) { ::glPopName(); }

  virtual void glPushAttrib(GLbitfield mask) { ::glPushAttrib(mask); }

  virtual void glPushMatrix(void) { ::glPushMatrix(); }

  virtual void glPushName(GLuint name) { ::glPushName(name); }

  virtual void glRasterPos3d(GLdouble x, GLdouble y, GLdouble z) { ::glRasterPos3d(x, y, z); }

  virtual void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
  {
    ::glReadPixels(x, y, width, height, format, type, pixels);
  }

  virtual GLint glRenderMode(GLenum mode) { return ::glRenderMode(mode); }

  virtual void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) { ::glRotated(angle, x, y, z); }

  virtual void glScaled(GLdouble x, GLdouble y, GLdouble z) { ::glScaled(x, y, z); }

  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z) { ::glScalef(x, y, z); }

  virtual void glSelectBuffer(GLsizei size, GLuint* buffer) { ::glSelectBuffer(size, buffer); }

  virtual void glShadeModel(GLenum mode) { ::glShadeModel(mode); }

  virtual void glTexCoord2d(GLdouble s, GLdouble t) { ::glTexCoord2d(s, t); }

  virtual void glTexImage2D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLenum format,
    GLenum type,
    GLvoid const* pixels
  )
  {
    ::glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
  }

  virtual void glTexParameteri(GLenum target, GLenum pname, GLint param) { ::glTexParameteri(target, pname, param); }

  virtual void glTranslated(GLdouble x, GLdouble y, GLdouble z) { ::glTranslated(x, y, z); }

  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z) { ::glTranslatef(x, y, z); }

  virtual void glVertex2d(GLdouble x, GLdouble y) { ::glVertex2d(x, y); }

  virtual void glVertex3d(GLdouble x, GLdouble y, GLdouble z) { ::glVertex3d(x, y, z); }

  virtual void glVertex3dv(GLdouble const* v) { ::glVertex3dv(v); }

  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) { ::glViewport(x, y, width, height); }
};
}  // namespace octave

#endif  // XEUS_OCTAVE_OPENGL_H
